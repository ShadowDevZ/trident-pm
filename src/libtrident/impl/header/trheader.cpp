
//#include "binarySerializer.h"

#include "trheader.h"
#include "libtrident.h"
using namespace Trd;
using namespace Trd::Consts::Header;
using eCode = Err::Code;
//we are intentionally not using sizeof()
//each different version of header will have different size
//we want to avoid the approach of the ms's solution with cbSize

//01.0.0
#define LT_HDR_VERSION_MIN 1000

const TRD_HEADER& TrFileHeader::getHeader() const {
    return trpkg.trdHdr;
}

//in future this might get overloaded with something like int version

std::expected<void, Err::TrdError> TrFileHeader::read() {
    auto optHdr = TrFileHeader::readBack(trpkg.fstrInfo);
    if (!optHdr.has_value()) {
        return std::unexpected(optHdr.error());
    }
    EXP_TRY(iValidateHeader(optHdr.value()));

    trpkg.trdHdr = optHdr.value();
    return {};
}

//reads back header, performs all field and validity checks, no need to call iValidateHeader
std::expected<TRD_HEADER, Err::TrdError> TrFileHeader::readBack(Impl::TStreamInfo& tStream) {
    TRD_HEADER hdr{};

    EXP_TRY(tStream.checkFileStreamInfo());

    auto readData = Impl::BinarySerializer::readDataFromTStream(tStream, TRD_HDR_START_OFFSET,
                                                                hdr.size(), true);
    if (!hdr.deserialize(readData)) {
        return std::unexpected(Err::TrdError(eCode::SerializerFailure));
    }

    EXP_TRY(iValidateHeader(hdr));

    return hdr;
}
bool TrFileHeader::isValid() {
    if (!trpkg.fstrInfo.checkFileStreamInfo())
        return false;
    return iValidateHeader(trpkg.trdHdr).has_value();
}

std::expected<void, Err::TrdError>
TrFileHeader::isHeaderPresent(Trd::Impl::TStreamInfo& streamInfo) {
    //returned header is already validated
    auto optHdr = TrFileHeader::readBack(streamInfo);
    if (!optHdr.has_value()) {
        return std::unexpected(optHdr.error());
    }

    return {};
}

std::expected<void, Err::TrdError> TrFileHeader::iValidateHeader(const TRD_HEADER& hdrIn) {

    [[unlikely]]
    if (hdrIn.size() != Consts::Header::LT_HDR_SZB_01A) {
        return std::unexpected(Err::TrdError(eCode::SectionSizeViolated));
    }

    if (hdrIn.magic != std::to_array(TRD_HDR_MAGIC) ||
        hdrIn.exSignature != TRD_HDR_EXTENDED_SIGNATURE) {

        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
    if (hdrIn.fmtVersion == Consts::Header::TRD_HDR_INVALID_VERSION) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    EXP_TRY(iCheckCRC(hdrIn));
    if (hdrIn.dynFileLen == 0) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    //todo check each field including signature
    return {};
}

std::expected<void, Err::TrdError> TrFileHeader::write() {
    if (!isValid()) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    auto& hdrStream = trpkg.fstrInfo;
    EXP_TRY(hdrStream.checkFileStreamInfo());

    //hdrStream.setSeekPos(TRD_HDR_START_OFFSET);
    auto serializer = trpkg.trdHdr.serialize();
    if (!serializer.has_value()) {
        return std::unexpected(Err::TrdError(eCode::SerializerFailure));
    }
    // hdrStream.writeTStream(serializer.value());
    Impl::BinarySerializer::writeDataToTStream(hdrStream, serializer.value(), true,
                                               TRD_HDR_START_OFFSET);

    return {};
}

std::expected<void, Err::TrdError> TrFileHeader::iCheckCRC(const TRD_HEADER& hdr) {
    auto genCrc = hdr.checksumCRC32();
    if (!genCrc)
        return std::unexpected(
            Err::TrdError(eCode::ChecksumFailure, 1, "no checksum was provided"));
    u32 cksum = genCrc.value().getCrc32();
    if ((cksum != hdr.dynHdrChksum)) {
        dbgprintf("[CRC_HDR] gen %u : exp: %u\n", cksum, hdr.dynHdrChksum);
        return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    }
    return {};
}

std::expected<void, Err::TrdError> TrFileHeader::create(BuildFlags::Flags buildFlgs,
                                                        ArchType::Type archType,
                                                        GlobalCompression::Algorithm comprType) {
    //return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    TRD_HDRFIELD_UPDATE update;
    auto fmtHdr = formatHeaderVersion(TRD_HDR_VMAJOR, TRD_HDR_VMINOR, TRD_HDR_VREVISION);
    if (!fmtHdr.has_value()) {
        return std::unexpected(fmtHdr.error());
    }
    update.buildFlags = buildFlgs;
    update.architecture = archType;
    update.compression = comprType;
    update.fmtVersion = fmtHdr.value();
    return create(update);
}
std::expected<void, Err::TrdError> TrFileHeader::create(const TRD_HDRFIELD_UPDATE& field) {

    if (field.fmtVersion == 0) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    TRD_HEADER hdr{};
    //todo probably instead call iValidateHeader()
    //the data is already assigned in struct, just a check if someone tried messing with it
    [[unlikely]]
    if (hdr.magic != std::to_array(TRD_HDR_MAGIC) ||
        hdr.exSignature != TRD_HDR_EXTENDED_SIGNATURE) {

        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }

    if (!(field.fmtVersion.has_value() || field.compression.has_value() ||
          field.buildFlags.has_value() || field.architecture.has_value())) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    hdr.fmtVersion = *field.fmtVersion;
    hdr.compression = *field.compression;
    hdr.buildFlags = *field.buildFlags;
    hdr.architecture = *field.architecture;
    // NOLINTEND(bugprone-unchecked-optional-access)
    auto genCrc = hdr.checksumCRC32();
    u32 crc = Consts::TRD_INVALID_CHKSUM;
    if (genCrc)
        crc = genCrc.value().getCrc32();

    hdr.dynHdrChksum = crc; //ignored for now

    hdr.dynFileLen = UINT64_MAX;
    hdr._reserved0 = 0;

    trpkg.trdHdr = hdr;
    return {};
}

std::expected<u16, Err::TrdError> TrFileHeader::formatHeaderVersion(u8 major, u8 minor,
                                                                    u8 revision) {
    if (major > 99 || minor > 99 || revision > 9 || major == 0) {
        return std::unexpected(Err::TrdError(Err::Code::InvalidFuncArg));
    }
    char dst[6];
    std::snprintf(dst, sizeof(dst), "%02u%02u%01u", major, minor, revision);
    const u16 r = (u16)std::strtoul(dst, nullptr, 10);
    //the r==0 is there only for ReadHeaderability because strtoul may return 0 on failure
    if (r == UINT16_MAX || r == 0 || r < LT_HDR_VERSION_MIN) {
        return std::unexpected(Err::TrdError(Err::Code::InvalidFuncArg));
    }
    return r;
}
std::expected<std::string, Err::TrdError>
TrFileHeader::headerVersionFormatToString(u16 fmt, bool abRevision) {
    std::string base;
    if (fmt < LT_HDR_VERSION_MIN) {
        return std::unexpected(Err::TrdError(Err::Code::InvalidFuncArg));
    }

    base = std::to_string(fmt);
    //if major is single digit full version is 1000
    //we need to append 0 at the beginning so we can split the string more easily
    if (base.length() == 4) {
        base = "0" + base;
    }
    const std::string major = base.substr(0, 2);
    const std::string minor = base.substr(2, 2);
    const u8 revision = std::stoi(base.substr(4));

    //std::stringstream ss;
    std::string strVersion;
    strVersion += major + "." + minor + ".";
    if (abRevision) {
        const char revName = 'A' + revision;
        strVersion += revName;
    } else {
        strVersion += std::to_string(revision);
    }

    return strVersion;
}
std::expected<void, Err::TrdError> TrFileHeader::updateHeader(const TRD_HDRFIELD_UPDATE& update) {
    TRD_HEADER hdr = trpkg.trdHdr;
    if (update.architecture)
        hdr.architecture = *update.architecture;
    if (update.fmtVersion)
        hdr.fmtVersion = *update.fmtVersion;
    if (update.compression)
        hdr.compression = *update.compression;
    if (update.buildFlags)
        hdr.buildFlags = *update.buildFlags;

    auto genCrc = hdr.checksumCRC32();

    if (!genCrc)
        return std::unexpected(
            Err::TrdError(eCode::ChecksumFailure, 1, "no checksum was provided"));

    hdr.dynHdrChksum = genCrc.value().getCrc32();

    auto val = iValidateHeader(hdr);
    if (!val.has_value()) {
        return std::unexpected(val.error());
    }
    trpkg.trdHdr = std::move(hdr);

    return TrFileHeader::write();
}
//CHECKSUM IS NOT UPDATED BECAUSE THESE ARE CONSIDERED DYNAMIC HEADER PROPS WHICH ARE NOT USED IN CRC FORMULA
//todo write directly

std::expected<void, Err::TrdError> TrFileHeader::updateFileLenProp(u64 len) {
    //todo check if valid
    trpkg.trdHdr.dynFileLen = len;
    return TrFileHeader::write();
}