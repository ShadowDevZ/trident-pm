#include "trheader.h"
#include "trdconsts.h"
#include "tstreaminfo.h"
#include <expected>

#include "sdesc.h"
#include "libtrident.h"
using namespace Trd;
using eCode = Err::Code;

std::expected<void, Err::TrdError> TrSectionDescriptor::createWriteBlank() {
    // header needs to exist before any sd data is written
    auto present = TrFileHeader::isHeaderPresent(trpkg.fstrInfo);
    if (!present) {
        return std::unexpected(present.error());
    }
    if (u8b_isTrue(trpkg.trdSD.sdReady))
        throw std::runtime_error("Cannot make SD blank as SD was marked with status READY");

    trpkg.trdSD = {};

    // todo issue a write

    return IwriteSDNoValidate(trpkg.trdSD);
};
std::expected<void, Err::TrdError> TrSectionDescriptor::write() {
    auto valid = iValidateSD(trpkg.trdSD, false);
    if (!valid.has_value())
        return std::unexpected(valid.error());

    auto sdVal = IwriteSDNoValidate(trpkg.trdSD);
    if (!sdVal.has_value())
        return std::unexpected(sdVal.error());

    return {};
}
void TrSectionDescriptor::changeReadyStatus(bool ready) {
    trpkg.trdSD.sdReady = ready;
}
bool TrSectionDescriptor::isReady() const {
    return trpkg.trdSD.sdReady;
}

std::expected<void, Err::TrdError>
TrSectionDescriptor::IwriteSDNoValidate(const Impl::TRD_SECTION_DESCRIPTOR& sd) {
    EXP_TRY(iFieldCheckSD(sd));

    auto& sdStream = trpkg.fstrInfo;
    EXP_TRY(sdStream.checkFileStreamInfo());

    auto serializer = sd.serialize();
    if (!serializer.has_value()) {
        return std::unexpected(Err::TrdError(eCode::SerializerFailure));
    }
    auto startOffset = getStartOffset();
    if (!startOffset.has_value())
        return std::unexpected(Err::TrdError(eCode::BadObject));

    Impl::BinarySerializer::writeDataToTStream(sdStream, serializer.value(), startOffset.value());

    return {};
}

// Of course its another copy from the header class, in future there will be interface
// for this (TM)
std::expected<void, Trd::Err::TrdError>
TrSectionDescriptor::iCheckCRC(const Impl::TRD_SECTION_DESCRIPTOR& sd) {
    const u32 genCrc = sd.checksumCRC32().value_or(Consts::TRD_INVALID_CHKSUM);
    if ((genCrc != sd.crc)) {
        dbgprintf("[CRC_SD] gen %u : exp: %u\n", genCrc, sd.crc);
        return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    }
    return {};
}

std::expected<void, Trd::Err::TrdError>
TrSectionDescriptor::iFieldCheckSD(const Impl::TRD_SECTION_DESCRIPTOR& sd) {
    [[unlikely]]
    if (sd.size() != Consts::SD::TRD_SECTIONSD_SIZE) {
        dbgprintf("sd size violation: exp: %u got: %lu\n", Consts::SD::TRD_SECTIONSD_SIZE,
                  sd.size());
        return std::unexpected(Err::TrdError(eCode::SectionSizeViolated));
    }
    if (sd._reserved1 != 0 || sd._reserved2 != 0) {
        return std::unexpected(Err::TrdError(eCode::ReservedFieldViolated));
    }
    if (!u8b_valid(sd.sdReady)) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    if (sd.idByte != Consts::SD::TRD_SD_IDBYTE) {
        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
    return {};
}
std::expected<void, Trd::Err::TrdError>
TrSectionDescriptor::iValidateSD(const Impl::TRD_SECTION_DESCRIPTOR& sd, bool checkReady) {
    auto x = iFieldCheckSD(sd);
    if (!x.has_value())
        return x;
    if (checkReady && !sd.sdReady) {
        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
    // if the SD is marked as ready then we are expecting all of these fields to be filled with existing
    // information, todo there should be a call to function which checks if the offsets are actually correct
    if (sd.sdReady && (sd.tblCount == 0 || sd.tblDynamicOffset == 0 || sd.tblRegistryOffset == 0)) {
        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
    auto crc = iCheckCRC(sd);
    if (!crc.has_value())
        return std::unexpected(crc.error());

    // todo once dyntbl and treg are imlpemented jump to each offsets and check section
    return {};
}

std::expected<u64, Err::TrdError> TrSectionDescriptor::getStartOffset() {
    return Consts::Header::LT_HDR_SZB_01A + 1;
}
std::expected<u64, Err::TrdError> TrSectionDescriptor::getEndOffset() {
    auto x = getStartOffset();
    if (!x.has_value())
        return std::unexpected(x.error());

    return x.value() + Consts::SD::TRD_SECTIONSD_SIZE;
}

const Impl::TRD_SECTION_DESCRIPTOR& TrSectionDescriptor::getSD() const {
    return trpkg.trdSD;
}

std::expected<void, Trd::Err::TrdError>
TrSectionDescriptor::updateSD(const Impl::TRD_SD_UPDATEFIELD& update,
                              std::optional<bool> setReadyStatus, bool checkReady) {
    Impl::TRD_SECTION_DESCRIPTOR sdTemp = trpkg.trdSD;
    if (update.sectionStatusCode)
        sdTemp.sectionStatusCode = *update.sectionStatusCode;
    if (update.tblCount)
        sdTemp.tblCount = *update.tblCount;
    if (update.tblDynamicOffset)
        sdTemp.tblDynamicOffset = *update.tblDynamicOffset;
    if (update.tblRegistryOffset)
        sdTemp.tblRegistryOffset = *update.tblRegistryOffset;

    sdTemp.crc = sdTemp.checksumCRC32().value_or(Consts::TRD_INVALID_CHKSUM);
    if (setReadyStatus)
        // because we are assigning u8b not bool
        sdTemp.sdReady = *setReadyStatus ? 1 : 0;

    auto optValid = iValidateSD(sdTemp, checkReady);
    if (!optValid) {
        return std::unexpected(optValid.error());
    }
    trpkg.trdSD = sdTemp;
    return TrSectionDescriptor::write();
}
bool TrSectionDescriptor::isValid() {
    if (!trpkg.fstrInfo.checkFileStreamInfo())
        return false;
    return iValidateSD(trpkg.trdSD).has_value();
}

std::expected<Impl::TRD_SECTION_DESCRIPTOR, Err::TrdError>
TrSectionDescriptor::readBack(Impl::TStreamInfo& tStream) {

    EXP_TRY(tStream.checkFileStreamInfo());
    Impl::TRD_SECTION_DESCRIPTOR sd{};
    auto startOffset = getStartOffset();
    auto endOffset = getEndOffset();

    const u64 size = endOffset.value() - startOffset.value();
    if (!startOffset || !endOffset || size != Consts::SD::TRD_SECTIONSD_SIZE)
        return std::unexpected(Err::TrdError(eCode::SectionSizeViolated));

    auto readData = Impl::BinarySerializer::readDataFromTStream(tStream, startOffset.value(), size);
    if (!sd.deserialize(readData)) {
        return std::unexpected(Err::TrdError(eCode::SerializerFailure));
    }
    EXP_TRY(iValidateSD(sd));
    return sd;
}

std::expected<void, Err::TrdError> TrSectionDescriptor::read() {
    auto optSD = readBack(trpkg.fstrInfo);
    if (!optSD) {
        return std::unexpected(optSD.error());
    }
    EXP_TRY(iValidateSD(optSD.value()));
    trpkg.trdSD = optSD.value();
    return {};
}
std::expected<void, Err::TrdError> TrSectionDescriptor::isSdPresent(Impl::TStreamInfo& streamInfo) {
    //sd returned is already validated no need to call iValidateSD()
    auto optSD = readBack(streamInfo);
    if (!optSD)
        return std::unexpected(optSD.error());

    return {};
}