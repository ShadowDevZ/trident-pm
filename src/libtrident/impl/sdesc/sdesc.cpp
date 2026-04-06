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

std::expected<void, Err::TrdError> TrSectionDescriptor::IwriteSDNoValidate(const Impl::TRD_SECTION_DESCRIPTOR& sd) {
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
std::expected<void, Trd::Err::TrdError> TrSectionDescriptor::iCheckCRC(const Impl::TRD_SECTION_DESCRIPTOR& sd) {
    const u32 genCrc = sd.checksumCRC32().value_or(Consts::TRD_INVALID_CHKSUM);
    if ((genCrc != sd.crc)) {
        dbgprintf("[CRC_SD] gen %u : exp: %u\n", genCrc, sd.crc);
        return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    }
    return {};
}

std::expected<void, Trd::Err::TrdError> TrSectionDescriptor::iFieldCheckSD(const Impl::TRD_SECTION_DESCRIPTOR& sd) {
    [[unlikely]]
    if (sd.size() != Consts::SD::TRD_SECTIONSD_SIZE) {
        dbgprintf("sd size violation: exp: %u got: %lu\n", Consts::SD::TRD_SECTIONSD_SIZE, sd.size());
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
std::expected<void, Trd::Err::TrdError> TrSectionDescriptor::iValidateSD(const Impl::TRD_SECTION_DESCRIPTOR& sd, bool checkReady) {
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

std::expected<void, Trd::Err::TrdError> TrSectionDescriptor::updateSD(const Impl::TRD_SD_UPDATEFIELD& update, std::optional<bool> setReadyStatus, bool checkReady) {
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

std::expected<Impl::TRD_SECTION_DESCRIPTOR, Err::TrdError> TrSectionDescriptor::readBack(Impl::TStreamInfo& tStream) {

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

// temporarily disabled for testing
/*
#include "sdesc.h"
#include "sdescid.h"
#include "trheader.h"
//#include "binarySerializer.h"
//#include "suid.h"
#include "trdconsts.h"
#include "tstreaminfo.h"
#include <cstring>
#include "trderr.h"
#include <zlib.h>
using namespace Trd::SectionDescriptor;

//using namespace Trd::UID;
using namespace Trd;
using eCode = Err::Code;
u32 IGenerateChecksum(const TRD_SD& sd);


//starting location of SD table without BEG/END token

//difference between these and raw functions is that Raw function point to the start of GUID whilst these point to actual data
//less error checking
//todo fix repetetiveness
constexpr foffset_t TRDSecDesc::GetSDAddress() noexcept {
    //todo actually find the TUID inside the stream and get its position to check presence start
   return TRDSdToken::GetOptRawSDStart() + Trd::Consts::SUID::SUID_MAX_LENGTH;
}
constexpr foffset_t TRDSecDesc::GetSDEnd()  noexcept{
    return GetSDAddress() + Trd::Consts::SD::TRD_SECTIONSD_SIZE;
}
//todo for normal write lookup the SUID using bmh algo from uid.cpp in future
std::expected<void, Err::TrdError> TRDSecDesc::WriteBlankSD() {
    //just in case there was some garbage before
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        return std::unexpected(Err::TrdError(eCode::ReferenceExpired));
    }
    auto sdStream = haveCtx.value();
    //to use std::filln we would have to write iterator
    std::memset(&secDescInternal, 0, sizeof(secDescInternal));
    //write SUID prologue
    TRDSdToken sdToken(sdStream);
    auto writeSDd = sdToken.WriteDescriptorSUID();
    if(!writeSDd.has_value()) {
        return std::unexpected(writeSDd.error());
    }

    return IWriteSD(true);
}


std::expected<void, Err::TrdError> TRDSecDesc::IWriteSD(bool blankWrite) {
    //we do not validate empty TRD_SD
    if (!blankWrite && !iValidateSDContent(secDescInternal)) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        return std::unexpected(Err::TrdError(eCode::ReferenceExpired));
    }
    auto sdStream = haveCtx.value();
    if (!sdStream->CheckFileStreamInfo()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
    }
    auto rwAccess = IRwAccessible(sdStream);
    if (!rwAccess.has_value()) {
        return std::unexpected(rwAccess.error());
    }

   // auto& fstrInfo = sdStream->GetFstreamObject();
   // auto& fstrStream = sdStream->GetFstreamObject().hFile;

    sdStream->SetSeekPosW(GetSDAddress());

    //const char* hdrContent = reinterpret_cast<const char*>(&secDescInternal);
    sdStream->WriteTStream<TRD_SD>(secDescInternal);

    return {};
}
//checks if we have header first
//std::expected<void, Err::TrdError> TRDSecDesc::IRwAccessible(){
    //todo check
 //  return IRwAccessible(wFstr);
//}
std::expected<void, Err::TrdError> TRDSecDesc::IRwAccessible(Trd::Tstream::TStreamInfo& fstr) {
    //todo check
    auto hdrStatus = TrFileHeader::IsHeaderPresent(fstr);
    if (!hdrStatus.has_value()) {
        return std::unexpected(hdrStatus.error());
    }
    //todo find if SUID tag is present
    return {};
}

std::expected<void, Err::TrdError> TRDSecDesc::UpdateSD(const TRD_SD_UPDATEFIELD& sd, bool autoWrite) {
    TRD_SD updateSd = secDescInternal;

    updateSd.tblCount = sd.tblCount;
    updateSd.tblDynamicOffset = sd.tblDynamicOffset;
    updateSd.tblRegistryOffset = sd.tblRegistryOffset;
    updateSd.crc = IGenerateChecksum(updateSd);
    auto sdValid = iValidateSDContent(updateSd);
    if (!sdValid.has_value()) {
        return std::unexpected(sdValid.error());
    }
    secDescInternal = updateSd;

    if (autoWrite) {
        return IWriteSD();
    }
    return {};
}

/*
if we are going to have to perform random access we probably should use mmap() syscall, in that
case we probably should also in Impl create custom std::ostream clonse for memory mapped file, mimicking the
seek/tell functions, this will however make it more os dependent and we are going to need multiple os specific function
prototypes. There is also problem that when we write to the file and size changes we have to again call mmap() because
the kernel won't update the size automatically resulting in SIGBUS, this becomes problematic. Perhaps in future we could
utilize header only cross platform library like https://github.com/vimpunk/mio
*/

/*
std::expected<void, Err::TrdError> TRDSecDesc::iValidateSDContent(const TRD_SD& sd) {

    if (!IChecksumValid(sd.crc, sd)) {
        return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    }
    if (sd._reserved0 != 0) {
        return std::unexpected(Err::Code(eCode::ReservedFieldViolated));
    }
    //todo check crc and fields, this will be done when we actually have dynamic section table
    return {};
}
std::expected<void, Err::TrdError> TRDSecDesc::IValidateTblAddr(const TRD_SD& sd) {
    if (sd.tblCount == 0 || sd.tblDynamicOffset == 0 || sd.tblRegistryOffset == 0) {
        //todo actually check each offset
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    return {};
}
u32 IGenerateChecksum(const TRD_SD& sd) {
    #define _LOCAL_CRC(crc,x) crc32(((crc)), reinterpret_cast<const Bytef*>(&(x)), sizeof((x)))
    u32 crc = ::crc32(0, Z_NULL, 0);
    crc = _LOCAL_CRC(crc, sd.tblCount);
    crc = _LOCAL_CRC(crc, sd.tblDynamicOffset);
    crc = _LOCAL_CRC(crc, sd.tblRegistryOffset);
    //do not generate for crc, it doesn't contain valid value yet
    return crc;
}
bool TRDSecDesc::IChecksumValid(u32 crc, const TRD_SD& sd) {
    u32 genCrc = IGenerateChecksum(sd);
    if ((genCrc != sd.crc) || (crc == 0)) {
        return false;
    }
    return true;
}
//These functions have to calculate CRC32 unlike the Header UpdateX
std::expected<void, Err::TrdError> TRDSecDesc::UpdateSDTblCount(u32 tblCount) {
    secDescInternal.tblCount = tblCount;
    secDescInternal.crc = IGenerateChecksum(secDescInternal);
    return IWriteSD();
}
std::expected<void, Err::TrdError>TRDSecDesc::UpdateSDDynOffset(u64 dynOffset) {
    secDescInternal.tblDynamicOffset = dynOffset;
    secDescInternal.crc = IGenerateChecksum(secDescInternal);
    return IWriteSD();
}
std::expected<void, Err::TrdError> TRDSecDesc::UpdateSDRegOffset(u64 tregOffset) {
    secDescInternal.tblRegistryOffset = tregOffset;
    secDescInternal.crc = IGenerateChecksum(secDescInternal);
    return IWriteSD();
}

std::expected<TRD_SD, Err::TrdError>TRDSecDesc::ReadBack() {
    TRD_SD sdDesc { };

    auto haveCtx= Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        return std::unexpected(Err::TrdError(eCode::ReferenceExpired));
    }
    auto sdStream = haveCtx.value();
    auto expStream = sdStream->CheckFileStreamInfo();

    if (!expStream.has_value()) {
        return std::unexpected(expStream.error());
    }


    i64 originalPosition = sdStream->GetSeekPosR();

    if (originalPosition == -1) {
        return std::unexpected(Err::TrdError(eCode::StreamSeekFailure));
    }
    auto rwAccess = IRwAccessible(sdStream);
    if (!rwAccess.has_value()) {
        return std::unexpected(rwAccess.error());
    }

    sdStream->SetSeekPosR(GetSDAddress());


   // ReadHeaderStream.ReadHeader(reinterpret_cast<char*>(&hdr), GetHeaderByteSize());

    sdStream->ReadTStream<TRD_SD>(sdDesc);


    sdStream->SetSeekPosR(originalPosition);
    auto valContent = iValidateSDContent(sdDesc);
    if (!valContent.has_value()) {
        return std::unexpected(valContent.error());
    }

    return sdDesc;
 }

 std::expected<void, Err::TrdError> TRDSecDesc::Read() {
    auto optHdr = TRDSecDesc::ReadBack();
    //todo call when implemented IValidateTblAddr
    if (!optHdr.has_value() || !iValidateSDContent(optHdr.value())) {
        secDescInternal = optHdr.value();
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    return {};
}

bool TRDSecDesc::IsValid() {
    auto have = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!have.has_value()) {
        return false;
    }
    if (!have.value()->IsOpen()) {
        return false;
    }
    /* use when actually properly implemented
    if (!IValidateTblAddr(secDescInternal)) {
        return false;
    }
    */
/*
    return iValidateSDContent(secDescInternal).has_value();
}


std::expected<void, Err::TrdError> TRDSecDesc::IsSDPresent() {
    return std::unexpected(Err::TrdError(eCode::FunctionNotImplemented));
}


*/