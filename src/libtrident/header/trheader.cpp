#include "trheader.h"
#include "libtrident.h"
#include <zlib.h>
//#include "pkgio.h"

using namespace LibTrident;
using namespace LibTrident::Consts::HeaderConsts;
using eCode = Err::Code;
//we are intentionally not using sizeof()
//each different version of header will have different size
//we want to avoid the approach of the ms's solution with cbSize

//01.0.0
#define LT_HDR_VERSION_MIN 1000









//in future this might get overloaded with something like int version

std::expected<void, Err::TrdError> TRDPkgHeader::Read() {
    auto optHdr = TRDPkgHeader::ReadBack();
    if (!optHdr.has_value() || !IValidateHeader(optHdr.value())) {
        return std::unexpected(Err::TrdError(eCode::FileReadFailure));
    }
    
    trpkg.hdrInternal = optHdr.value();
    return {};
}



std::expected<TRD_HEADER, Err::TrdError>TRDPkgHeader::ReadBack() {
    TRD_HEADER hdr { };
    auto& hdrStream = trpkg.fstrInfo;
    if (!hdrStream.CheckFileStreamInfo().has_value()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
    }
    
   // auto& fstrStream = hdrStream->GetFstreamObject();
    
    i64 originalPosition = hdrStream.GetSeekPosR();

    if (originalPosition == -1) {
        return std::unexpected(Err::TrdError(eCode::StreamSeekFailure));
    }

    hdrStream.SetSeekPosR(TRD_HDR_START_OFFSET);
   
    if (!ICheckHeaderSize(trpkg.hdrInternal)) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    
   // ReadHeaderStream.ReadHeader(reinterpret_cast<char*>(&hdr), GetHeaderByteSize());

    hdrStream.ReadTStream<TRD_HEADER>(hdr);

    hdrStream.SetSeekPosR(originalPosition);
       
    if (!IValidateHeader(hdr)) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    
    return hdr;
}
bool TRDPkgHeader::IsValid() {
    if (!trpkg.fstrInfo.IsOpen()) {
        return false;
    }
    return IValidateHeader(trpkg.hdrInternal).has_value();
}
const TRD_HEADER& TRDPkgHeader::GetObject() const {
    return trpkg.hdrInternal;
}
TRD_HEADER& TRDPkgHeader::GetObject() {
    return trpkg.hdrInternal;
}

//UNSAFE WIP, always returns true
std::expected<void, Err::TrdError> TRDPkgHeader::IsHeaderPresent(LibTrident::Tstream::TStreamInfo& streamInfo) {
    /*
    if (!streamInfo.CheckFileStreamInfo().has_value()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
    }

    TRDPkgHeader hdr(fStreamInfo);
   
    auto optHdr = hdr.ReadBack();
    if (!optHdr.has_value()) {
        return std::unexpected(Err::TrdError(Err::Code::FileReadFailure));
    }
    auto valHdr = hdr.IValidateHeader(optHdr.value());
    if (!valHdr.has_value()) {
        dbgprintf("header not present\n");
        return std::unexpected(valHdr.error());
        
    }
    */
    return {};
}



std::expected<void, Err::TrdError> TRDPkgHeader::IValidateHeader(const TRD_HEADER& hdrIn) {
    if (!ICheckHeaderSize(hdrIn)) {
        return std::unexpected(Err::TrdError(eCode::SectionMissing));
    }
    if (!std::equal(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdrIn.magic))) {
        return std::unexpected(Err::TrdError(eCode::SectionMissing));
    }
    if (hdrIn.exSignature != TRD_HDR_EXTENDED_SIGNATURE) {
        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
    if (hdrIn.fmtVersion == 0) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    if (!ICheckCRC(hdrIn.hdrChksum, hdrIn)) {
        return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    }
    if (hdrIn.fileLen == 0) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    //todo check each field including signature
    return {};
}

std::expected<void, Err::TrdError> TRDPkgHeader::Write() {
    if (!ICheckHeaderSize(trpkg.hdrInternal)) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    if (!IsValid()) {
        return std::unexpected(Err::TrdError(eCode::BadObject));
    }
    auto& hdrStream = trpkg.fstrInfo;
    if (!hdrStream.CheckFileStreamInfo()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
       
    }
    //auto& fstrInfo = hdrStream->GetFstreamObject();
   


   // const char* hdrContent = reinterpret_cast<const char*>(&hdrInteral);
  //  streamInfo->hFile->Write(hdrContent, GetHeaderByteSize());
    
    hdrStream.SetSeekPosW(TRD_HDR_START_OFFSET);
    hdrStream.WriteTStream<TRD_HEADER>(trpkg.hdrInternal);
   
    return {};
}


bool TRDPkgHeader::ICheckHeaderSize(const TRD_HEADER& hdr) {
    if (sizeof(hdr) != LT_HDR_SZB_01A) {
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
    if ((genCrc != hdr.hdrChksum) || (crc == 0)) {
       return std::unexpected(Err::TrdError(eCode::ChecksumFailure));
    } 
    return {};
}


std::expected<void, Err::TrdError> TRDPkgHeader::Create(u32 buildFlgs, u8 archType, u8 comprType) {
    TRD_HDRFIELD_UPDATE update;
    auto fmtHdr = FormatHeaderVersion(TRD_HDR_VMAJOR, TRD_HDR_VMINOR, TRD_HDR_VREVISION);
    if (!fmtHdr.has_value()) {
        return std::unexpected(fmtHdr.error());
    }
    update.buildFlags = buildFlgs;
    update.architecture = archType;
    update.compression = comprType;
    update.fmtVersion = fmtHdr.value();
    return Create(update);
}
std::expected<void, Err::TrdError> TRDPkgHeader::Create(const TRD_HDRFIELD_UPDATE& field) {
    TRD_HEADER hdr;
    std::copy(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdr.magic));
    if (!std::equal(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdr.magic))) {
        return std::unexpected(Err::TrdError(eCode::SectionCorrupted));
    }
    hdr.exSignature = TRD_HDR_EXTENDED_SIGNATURE;
    hdr.fmtVersion = field.fmtVersion;
    if (hdr.fmtVersion == 0) {
        return std::unexpected(Err::TrdError(eCode::InvalidFuncArg));
    }
    hdr.compression = static_cast<u8>(field.compression);
    hdr.buildFlags = static_cast<u32>(field.buildFlags);
    hdr.architecture = static_cast<u8>(field.architecture);
    hdr.hdrChksum = IGenerateHeaderCRC(hdr);
    
    hdr.fileLen = UINT64_MAX;
    hdr.ioCtrl = 0;

    trpkg.hdrInternal = hdr;
    return {};
}

std::expected<u16, Err::TrdError> TRDPkgHeader::FormatHeaderVersion(u8 major, u8 minor, u8 revision) {
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
std::expected<std::string, Err::TrdError> TRDPkgHeader::HeaderVersionFormatToString(u16 fmt, bool abRevision) {
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
std::expected<void, Err::TrdError> TRDPkgHeader::UpdateHeader(const TRD_HDRFIELD_UPDATE& update) {
    TRD_HEADER hdr = trpkg.hdrInternal;
    hdr.architecture = static_cast<u8>(update.architecture);
    hdr.fmtVersion = update.fmtVersion;
    hdr.compression = static_cast<u8>(update.compression);
    hdr.buildFlags = static_cast<u32>(update.buildFlags);
    hdr.hdrChksum = IGenerateHeaderCRC(hdr);

    auto val = IValidateHeader(hdr);
    if (!val.has_value()) {
        return std::unexpected(val.error());
    } 
    trpkg.hdrInternal = hdr;
    
    return TRDPkgHeader::Write();
 }
//CHECKSUM IS NOT UPDATED BECAUSE THESE ARE CONSIDERED DYNAMIC HEADER PROPS WHICH ARE NOT USED IN CRC FORMULA 
//todo write directly
std::expected<void, Err::TrdError> TRDPkgHeader::UpdateIoctrlProp(u16 ioctrl) {
    //todo check if valid
    trpkg.hdrInternal.ioCtrl = ioctrl;
    return TRDPkgHeader::Write();
}

std::expected<void, Err::TrdError> TRDPkgHeader::UpdateFileLenProp(u64 len) {
    //todo check if valid
    trpkg.hdrInternal.fileLen = len;
    return TRDPkgHeader::Write();

 }