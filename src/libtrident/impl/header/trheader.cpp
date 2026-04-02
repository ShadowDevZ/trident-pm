#include <zlib.h>
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
    if (!optHdr.has_value() || !iValidateHeader(optHdr.value())) {
        return std::unexpected(Err::TrdError(eCode::FileReadFailure));
    }
    
    trpkg.trdHdr = optHdr.value();
    return {};
}


//reads back header, performs all field and validity checks, no need to call iValidateHeader
std::expected<TRD_HEADER, Err::TrdError>TrFileHeader::readBack(Impl::TStreamInfo& tStream) {
    TRD_HEADER hdr {};
  
    if (!tStream.checkFileStreamInfo().has_value()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
    }

    auto readData =  Impl::BinarySerializer::readDataFromTStream(tStream, TRD_HDR_START_OFFSET, hdr.size());
    if (!hdr.deserialize(readData)) {
        return std::unexpected(Err::TrdError(eCode::SerializerFailure));
    }
   
       
    if (!iValidateHeader(hdr)) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    
    return hdr;
}
bool TrFileHeader::isValid() {
    if (!trpkg.fstrInfo.isOpen()) {
        return false;
    }
    return iValidateHeader(trpkg.trdHdr).has_value();
}


std::expected<void, Err::TrdError> TrFileHeader::isHeaderPresent(Trd::Impl::TStreamInfo& streamInfo) {
   
    if (!streamInfo.checkFileStreamInfo().has_value()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
    }
   
    auto optHdr = TrFileHeader::readBack(streamInfo);
    if (!optHdr.has_value()) {
        return std::unexpected(Err::TrdError(Err::Code::FileReadFailure));
    }
    
    return {};
}



std::expected<void, Err::TrdError> TrFileHeader::iValidateHeader(const TRD_HEADER& hdrIn){

   

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
    if (!iCheckCRC(hdrIn).has_value()) {
        return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    }
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
    if (!hdrStream.checkFileStreamInfo()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
       
    }
    
    //hdrStream.setSeekPos(TRD_HDR_START_OFFSET);
    auto serializer = trpkg.trdHdr.serialize();
    if (!serializer.has_value()) {
        return std::unexpected(Err::TrdError(eCode::SerializerFailure));
    }
   // hdrStream.writeTStream(serializer.value());
    Impl::BinarySerializer::writeDataToTStream(hdrStream, serializer.value(), TRD_HDR_START_OFFSET);
   
    return {};
}


std::expected<void, Err::TrdError> TrFileHeader::iCheckCRC(const TRD_HEADER& hdr) {
    u32 genCrc = hdr.checksumCRC32().value_or(Consts::TRD_INVALID_CHKSUM);
    if ((genCrc != hdr.dynHdrChksum)) {
        dbgprintf("[CRC_HDR] gen %u : exp: %u\n", genCrc, hdr.dynHdrChksum);
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
    TRD_HEADER hdr {};
    //todo probably instead call iValidateHeader()
    //the data is already assigned in struct, just a check if someone tried messing with it
    [[unlikely]]
    if (hdr.magic != std::to_array(TRD_HDR_MAGIC) ||
        hdr.exSignature != TRD_HDR_EXTENDED_SIGNATURE) {

        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
   
    if (!  (field.fmtVersion.has_value() || field.compression.has_value() ||
            field.buildFlags.has_value() || field.architecture.has_value())) {
                return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    hdr.fmtVersion = *field.fmtVersion;
    hdr.compression = *field.compression;
    hdr.buildFlags = *field.buildFlags;
    hdr.architecture = *field.architecture;
    hdr.dynHdrChksum = hdr.checksumCRC32().value_or(Consts::TRD_INVALID_CHKSUM); //ignored for now
    
    hdr.dynFileLen = UINT64_MAX;
    hdr._reserved0 = 0;

    trpkg.trdHdr = hdr;
    return {};
}

std::expected<u16, Err::TrdError> TrFileHeader::formatHeaderVersion(u8 major, u8 minor, u8 revision) {
    if (major > 99 || minor > 99 || revision > 9
        || major == 0) {
        return std::unexpected(Err::TrdError(Err::Code::InvalidFuncArg));
    }
    char dst[6];
    std::snprintf(dst, sizeof(dst), "%02u%02u%01u", major, minor, revision);
    u16 r = (u16)std::strtoul(dst, NULL, 10);
    //the r==0 is there only for ReadHeaderability because strtoul may return 0 on failure
    if (r == UINT16_MAX || r == 0 || r < LT_HDR_VERSION_MIN) {
        return std::unexpected(Err::TrdError(Err::Code::InvalidFuncArg));
    }
    return r;
    

}
std::expected<std::string, Err::TrdError> TrFileHeader::headerVersionFormatToString(u16 fmt, bool abRevision) {
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
    std::string major  = base.substr(0,2);
    std::string minor =  base.substr(2,2);
    u8 revision = std::stoi(base.substr(4));

    
    //std::stringstream ss;
    std::string strVersion;
    strVersion += major + "." + minor + ".";
    if (abRevision) {
        char revName = 'A' + revision;
        strVersion += revName;
    }
    else {
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
    
    hdr.dynHdrChksum = hdr.checksumCRC32().value_or(Consts::TRD_INVALID_CHKSUM);
    

    auto val = iValidateHeader(hdr);
    if (!val.has_value()) {
        return std::unexpected(val.error());
    } 
    trpkg.trdHdr = hdr;
    
    return TrFileHeader::write();
 }
//CHECKSUM IS NOT UPDATED BECAUSE THESE ARE CONSIDERED DYNAMIC HEADER PROPS WHICH ARE NOT USED IN CRC FORMULA 
//todo write directly

std::expected<void, Err::TrdError> TrFileHeader::updateFileLenProp(u64 len) {
    //todo check if valid
    trpkg.trdHdr.dynFileLen = len;
    return TrFileHeader::write();

 }