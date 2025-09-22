#include "trheader.h"
#include <sstream>
#include <zlib.h>
#include "pkgio.h"

using namespace LibTrident;
using namespace LibTrident::Header;
using namespace LibTrident::Consts::HeaderConsts;

//we are intentionally not using sizeof()
//each different version of header will have different size
//we want to avoid the approach of the ms's solution with cbSize

//01.0.0
#define LT_HDR_VERSION_MIN 1000









//in future this might get overloaded with something like int version

bool TRDPkgHeader::ReadHeader() {
    auto optHdr = ReadHeaderBack();
    if (optHdr.has_value() && IValidateHeader(optHdr.value())) {
        hdrInteral = optHdr.value();
        return true;
    }
    
    return false;
}



std::optional<TRD_HEADER> TRDPkgHeader::ReadHeaderBack() {
    TRD_HEADER hdr { };
   
    auto haveCtx= Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        e.SetError(Err::Code::IREF_EXPIRED);
        return std::nullopt;
    }
    auto hdrStream = haveCtx.value();
    
    
    if (!hdrStream->CheckFileStreamInfo()) {
        e.SetError(Err::Code::NULL_OBJ);
        return std::nullopt;
    }
    
   // auto& fstrStream = hdrStream->GetFstreamObject();
    
    u64 originalPosition = hdrStream->GetSeekPosR();

    if (!hdrStream->e.IsOk() || originalPosition == -1) {
        e.SetError(Err::Code::FSEEK);
        return std::nullopt;
    }

    if (!hdrStream->SetSeekPosR(TRD_HDR_START_OFFSET)) {
        e.SetError(Err::Code::FSEEK);
        return std::nullopt;
    }
   
    if (!ICheckHeaderSize(hdrInteral)) {
        e.SetError(Err::Code::FSECCRP);
        return std::nullopt;
    }
    
   // ReadHeaderStream.ReadHeader(reinterpret_cast<char*>(&hdr), GetHeaderByteSize());

    bool ReadHeaderStatus = hdrStream->ReadTStream<TRD_HEADER>(hdr);
    if (!ReadHeaderStatus) {
        e.SetError(Err::Code::IO_READ);
        return std::nullopt;
    }

    if (!hdrStream->SetSeekPosR(originalPosition)) {
        e.SetError(Err::Code::FSEEK);
        return std::nullopt;
    }
    if (!IValidateHeader(hdr)) {
        return std::nullopt;
    }
    e.Success();
    
    return hdr;
}
bool TRDPkgHeader::IsValid() {
    auto have = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!have.has_value()) {
        return false;
    }
    if (!have.value()->IsOpen()) {
        return false;
    }
    return IValidateHeader(hdrInteral);
}

Err::Code TRDPkgHeader::IsHeaderPresent(std::weak_ptr<LibTrident::Tstream::TStreamInfo> streamInfo) {
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(streamInfo);
    if (!haveCtx.has_value()) {
        return Err::Code::IREF_EXPIRED;
    }
    auto fStreamInfo = haveCtx.value();

    TRDPkgHeader hdr(fStreamInfo);
   
    auto optHdr = hdr.ReadHeaderBack();
    Err::Code errCode = hdr.e.GetError();
    if (!optHdr.has_value()) {
        return errCode;
    }
    if (errCode != Err::Code::SUCCESS || !hdr.IValidateHeader(optHdr.value())) {
        dbgprintf("header not present\n");
        return errCode;
    }
    
    return Err::Code::SUCCESS;

}



bool TRDPkgHeader::IValidateHeader(const TRD_HEADER& hdrIn) {
    if (!ICheckHeaderSize(hdrIn)) {
        e.SetError(Err::Code::FSECNP);
        return false;
    }
    if (!std::equal(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdrIn.magic))) {
        e.SetError(Err::Code::FSECNP);
        return false;
    }
    if (hdrIn.exSignature != TRD_HDR_EXTENDED_SIGNATURE) {
       e.SetError(Err::Code::FSECCRP);
        return false;
    }
    if (hdrIn.fmtVersion == 0) {
        e.SetError(Err::Code::BADARG);
        return false;
    }
    if (!ICheckCRC(hdrIn.hdrChksum, hdrIn)) {
        e.SetError(Err::Code::CHKSUM);
        return false;
    }
    if (hdrIn.fileLen == 0) {
        e.SetError(Err::Code::BADARG);
        return false;
    }
    //todo check each field including signature
    e.Success();
    return true;
}

bool TRDPkgHeader::WriteHeader() {
    if (!ICheckHeaderSize(hdrInteral)) {
        e.SetError(Err::Code::FSECCRP);
        return false;
    }
    if (!IsValid()) {
        return false;
    }
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        e.SetError(Err::Code::IREF_EXPIRED);
        return false;
    }
    auto hdrStream = haveCtx.value();
    if (!hdrStream->CheckFileStreamInfo()) {
        e.SetError(Err::Code::NULL_OBJ);
        return false;
    }
    //auto& fstrInfo = hdrStream->GetFstreamObject();
   


   // const char* hdrContent = reinterpret_cast<const char*>(&hdrInteral);
  //  streamInfo->hFile->WriteHeader(hdrContent, GetHeaderByteSize());
    
    if (!hdrStream->SetSeekPosW(TRD_HDR_START_OFFSET)) {
        e.SetError(Err::Code::FSEEK);
        return false;
    }
    bool WriteHeaderStatus = hdrStream->WriteTStream<TRD_HEADER>(hdrInteral);
   
    if (!hdrStream->e.IsOk() || !WriteHeaderStatus) {
        e.SetError(Err::Code::IO_WRITE);
        return false;
    }
    

    return true;
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
bool TRDPkgHeader::ICheckCRC(u32 crc, const TRD_HEADER& hdr) {
    u32 genCrc = IGenerateHeaderCRC(hdr);
    if ((genCrc != hdr.hdrChksum) || (crc == 0)) {
        e.SetError(Err::Code::CHKSUM);
        return false;
    } 
    return true;
}


bool TRDPkgHeader::Create(u32 buildFlgs, u8 archType, u8 comprType) {
    TRD_HDRFIELD_UPDATE update;
    update.buildFlags = buildFlgs;
    update.architecture = archType;
    update.compression = comprType;
    update.fmtVersion = FormatHeaderVersion(TRD_HDR_VMAJOR, TRD_HDR_VMINOR, TRD_HDR_VREVISION);
    return Create(update);
}
bool TRDPkgHeader::Create(const TRD_HDRFIELD_UPDATE& field) {
    TRD_HEADER hdr;
    std::copy(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdr.magic));
    if (!std::equal(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdr.magic))) {
        e.SetError(Err::Code::COPYOBJ);
        return false;
    }
    hdr.exSignature = TRD_HDR_EXTENDED_SIGNATURE;
    hdr.fmtVersion = field.fmtVersion;
    if (hdr.fmtVersion == 0) {
        e.SetError(Err::Code::BADARG);
        return false;
    }
    hdr.compression = static_cast<u8>(field.compression);
    hdr.buildFlags = static_cast<u32>(field.buildFlags);
    hdr.architecture = static_cast<u8>(field.architecture);
    hdr.hdrChksum = IGenerateHeaderCRC(hdr);
    
    hdr.fileLen = UINT64_MAX;
    hdr.ioCtrl = 0;

    hdrInteral = hdr;
    return true;
}

u16 TRDPkgHeader::FormatHeaderVersion(u8 major, u8 minor, u8 revision) {
    if (major > 99 || minor > 99 || revision > 9
        || major == 0) {
        return 0;
    }
    char dst[6];
    std::snprintf(dst, sizeof(dst), "%02u%02u%01u", major, minor, revision);
    u16 r = (u16)std::strtoul(dst, NULL, 10);
    //the r==0 is there only for ReadHeaderability because strtoul may return 0 on failure
    if (r == UINT16_MAX || r == 0 || r < LT_HDR_VERSION_MIN) {
        return 0;
    }
    return r;
    

}
std::string TRDPkgHeader::HeaderVersionFormatToString(u16 fmt, bool abRevision) {
    std::string base;
    if (fmt < LT_HDR_VERSION_MIN) {
        return base;
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

    
    std::stringstream ss;
    ss << major << "." << minor << ".";
    if (abRevision) {
        char revName = 'A' + revision;
        ss << revName;
    }
    else {
        ss << std::to_string(revision);
    }
    

    return ss.str();

}
 bool TRDPkgHeader::UpdateHeader(const TRD_HDRFIELD_UPDATE& update) {
    TRD_HEADER hdr = hdrInteral;
    hdr.architecture = static_cast<u8>(update.architecture);
    hdr.fmtVersion = update.fmtVersion;
    hdr.compression = static_cast<u8>(update.compression);
    hdr.buildFlags = static_cast<u32>(update.buildFlags);
    hdr.hdrChksum = IGenerateHeaderCRC(hdr);

   
    if (!IValidateHeader(hdr)) {
        return false;
    } 
    hdrInteral = hdr;
    e.Success();
    
    return WriteHeader();
 }
//CHECKSUM IS NOT UPDATED BECAUSE THESE ARE CONSIDERED DYNAMIC HEADER PROPS WHICH ARE NOT USED IN CRC FORMULA 
//todo write directly
 bool TRDPkgHeader::UpdateIoctrlProp(u16 ioctrl) {
    //todo check if valid
    hdrInteral.ioCtrl = ioctrl;
    return WriteHeader();
}

 bool TRDPkgHeader::UpdateFileLenProp(u64 len) {
    //todo check if valid
    hdrInteral.fileLen = len;
    return WriteHeader();

 }