#include "trheader.h"
#include "libtrident.h"
#include <zlib.h>
//#include "binarySerializer.h"

using namespace LibTrident;
using namespace LibTrident::Consts::HeaderConsts;
using eCode = Err::Code;
//we are intentionally not using sizeof()
//each different version of header will have different size
//we want to avoid the approach of the ms's solution with cbSize

//01.0.0
#define LT_HDR_VERSION_MIN 1000



const TRD_HEADER& TRDPkgHeader::getHeader() const {
    return trpkg.trdHdr;
}






//in future this might get overloaded with something like int version

std::expected<void, Err::TrdError> TRDPkgHeader::read() {
    auto optHdr = TRDPkgHeader::readBack(trpkg.fstrInfo);
    if (!optHdr.has_value() || !IValidateHeader(optHdr.value())) {
        return std::unexpected(Err::TrdError(eCode::FileReadFailure));
    }
    
    trpkg.trdHdr = optHdr.value();
    return {};
}


//reads back header, performs all field and validity checks, no need to call IValidateHeader
std::expected<TRD_HEADER, Err::TrdError>TRDPkgHeader::readBack(Impl::TStreamInfo& tStream) {
    TRD_HEADER hdr {};
  
    if (!tStream.checkFileStreamInfo().has_value()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
    }
    
    
    //check size if someone accidentally decided to change some field
    [[unlikely]]
    if (!ICheckHeaderSize(hdr)) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    
  

    auto readData =  Impl::BinarySerializer::readDataFromTStream(tStream, TRD_HDR_START_OFFSET, hdr.size());
    if (!hdr.deserialize(readData)) {
        return std::unexpected(Err::TrdError(eCode::SerializerFailure));
    }
   
       
    if (!IValidateHeader(hdr)) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    
    return hdr;
}
bool TRDPkgHeader::isValid() {
    if (!trpkg.fstrInfo.isOpen()) {
        return false;
    }
    return IValidateHeader(trpkg.trdHdr).has_value();
}


std::expected<void, Err::TrdError> TRDPkgHeader::isHeaderPresent(LibTrident::Impl::TStreamInfo& streamInfo) {
   
    if (!streamInfo.checkFileStreamInfo().has_value()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
    }
   
    auto optHdr = TRDPkgHeader::readBack(streamInfo);
    if (!optHdr.has_value()) {
        return std::unexpected(Err::TrdError(Err::Code::FileReadFailure));
    }
    
    return {};
}



std::expected<void, Err::TrdError> TRDPkgHeader::IValidateHeader(const TRD_HEADER& hdrIn){

    return {}; //for debugging purposes as we dont have CRC implemented

    [[unlikely]]
    if (!ICheckHeaderSize(hdrIn)) {
        return std::unexpected(Err::TrdError(eCode::SectionMissing));
    }
     //the data is already assigned in struct, just a check if someone tried messing with it
    [[unlikely]]
    if (hdrIn.magic != std::to_array(TRD_HDR_MAGIC) ||
        hdrIn.exSignature != TRD_HDR_EXTENDED_SIGNATURE) {

        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
    if (hdrIn.fmtVersion == 0) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    if (!ICheckCRC(hdrIn.dynHdrChksum, hdrIn)) {
        return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    }
    if (hdrIn.dynFileLen == 0) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    //todo check each field including signature
    return {};
}

std::expected<void, Err::TrdError> TRDPkgHeader::write() {
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


bool TRDPkgHeader::ICheckHeaderSize(const TRD_HEADER& hdr) {
    if (hdr.size() != LT_HDR_SZB_01A) {
        return false;
    }
    return true;
}
//I mean we could technically dump here the raw pointer but is it really the best approach for very few fields ?
//todo probably implement serialize() function to each section which converts all elements to std::vector so we don't have to
//be repetetive, todo template
u32 IGenerateHeaderCRC(const TRD_HEADER& hdr) {
    #define _LOCAL_CRC(crc,x) crc32(((crc)), reinterpret_cast<const Bytef*>(&(x)), sizeof((x)))
    u32 crc = ::crc32(0, Z_NULL, 0);
    crc = _LOCAL_CRC(crc, hdr.magic);
    crc = _LOCAL_CRC(crc, hdr.exSignature);
    crc = _LOCAL_CRC(crc, hdr.fmtVersion);
    crc = _LOCAL_CRC(crc, hdr.compression);
    crc = _LOCAL_CRC(crc, hdr.buildFlags);
    crc = _LOCAL_CRC(crc, hdr.architecture);
    return crc;
}
std::expected<void, Err::TrdError> TRDPkgHeader::ICheckCRC(u32 crc, const TRD_HEADER& hdr) {
    u32 genCrc = IGenerateHeaderCRC(hdr);
    if ((genCrc != hdr.dynHdrChksum) || (crc == 0)) {
       return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    } 
    return {};
}


std::expected<void, Err::TrdError> TRDPkgHeader::create(u32 buildFlgs, u8 archType, u8 comprType) {
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
std::expected<void, Err::TrdError> TRDPkgHeader::create(const TRD_HDRFIELD_UPDATE& field) {
    
    if (field.fmtVersion == 0) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    TRD_HEADER hdr {};
    //todo probably instead call IValidateHeader()
    //the data is already assigned in struct, just a check if someone tried messing with it
    [[unlikely]]
    if (hdr.magic != std::to_array(TRD_HDR_MAGIC) ||
        hdr.exSignature != TRD_HDR_EXTENDED_SIGNATURE) {

        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
   

    hdr.fmtVersion = field.fmtVersion;
    hdr.compression = static_cast<u8>(field.compression);
    hdr.buildFlags = static_cast<u32>(field.buildFlags);
    hdr.architecture = static_cast<u8>(field.architecture);
    hdr.dynHdrChksum = 0; //ignored for now
   // hdr.dynHdrChksum = IGenerateHeaderCRC(hdr);
    
    hdr.dynFileLen = UINT64_MAX;
    hdr.dynIoCtrl = 0;

    trpkg.trdHdr = hdr;
    return {};
}

std::expected<u16, Err::TrdError> TRDPkgHeader::formatHeaderVersion(u8 major, u8 minor, u8 revision) {
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
std::expected<std::string, Err::TrdError> TRDPkgHeader::headerVersionFormatToString(u16 fmt, bool abRevision) {
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
std::expected<void, Err::TrdError> TRDPkgHeader::updateHeader(const TRD_HDRFIELD_UPDATE& update) {
    TRD_HEADER hdr = trpkg.trdHdr;
    hdr.architecture = static_cast<u8>(update.architecture);
    hdr.fmtVersion = update.fmtVersion;
    hdr.compression = static_cast<u8>(update.compression);
    hdr.buildFlags = static_cast<u32>(update.buildFlags);
   // hdr.dynHdrChksum = IGenerateHeaderCRC(hdr);
    hdr.dynHdrChksum = 0; //ignored for now

    auto val = IValidateHeader(hdr);
    if (!val.has_value()) {
        return std::unexpected(val.error());
    } 
    trpkg.trdHdr = hdr;
    
    return TRDPkgHeader::write();
 }
//CHECKSUM IS NOT UPDATED BECAUSE THESE ARE CONSIDERED DYNAMIC HEADER PROPS WHICH ARE NOT USED IN CRC FORMULA 
//todo write directly
std::expected<void, Err::TrdError> TRDPkgHeader::updateIoctrlProp(u16 ioctrl) {
    //todo check if valid
    trpkg.trdHdr.dynIoCtrl = ioctrl;
    return TRDPkgHeader::write();
}

std::expected<void, Err::TrdError> TRDPkgHeader::updateFileLenProp(u64 len) {
    //todo check if valid
    trpkg.trdHdr.dynFileLen = len;
    return TRDPkgHeader::write();

 }