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
    TRD_HEADER hdr;
    bool status = ReadHeaderBack(hdr);
    if (status && ValidateHeader(hdr) == LTSTATUS::SUCCESS) {
        hdrInteral = hdr;
        return true;
    }
   
    return false;
}



bool TRDPkgHeader::ReadHeaderBack(TRD_HEADER& hdrOut) {
    TRD_HEADER hdr { };
    hdrOut = hdr;
    auto [checkWeakRef, sharedPtr] = FstreamInfo::TrdFstreamInfo::GetFstreamContent(wFstr);
    if (!checkWeakRef) {
        e.SetError(LTSTATUS::IREF_EXPIRED);
        return false;
    }
    if (!sharedPtr->CheckFileStreamInfo()) {
        e.SetError(LTSTATUS::NULL_OBJ);
        return false;
    }
    auto& fstrInfo = sharedPtr->GetFstreamObject();
    auto& fstrStream = fstrInfo.hFile;
    
    std::streampos originalPosition = fstrStream->tellg();
    fstrStream->seekg(TRD_HDR_START_OFFSET, std::ios::beg);
    if (!fstrStream) {
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    if (!ICheckHeaderSize(hdrInteral)) {
        e.SetError(LTSTATUS::HDRCRP);
        return false;
    }
    
   // ReadHeaderStream.ReadHeader(reinterpret_cast<char*>(&hdr), GetHeaderByteSize());

    bool ReadHeaderStatus = PkgIO::FileOperations::ReadLeStream(fstrInfo, reinterpret_cast<char*>(&hdr), LT_HDR_SZB_01A);
    if (!fstrStream|| !ReadHeaderStatus) {
        e.SetError(LTSTATUS::IO_READ);
        return false;
    }

    fstrStream->seekg(originalPosition, std::ios::beg);
    if (!fstrStream) {
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    if (ValidateHeader(hdr) != LTSTATUS::SUCCESS) {
        return false;
    }
    e.Success();
    hdrOut = hdr;
    return true;
}
bool TRDPkgHeader::IsValid() {
    LTSTATUS::LTSTATUS status = ValidateHeader(hdrInteral);
    e.SetError(status);
    if (status == LTSTATUS::SUCCESS){
        return true;
    }
    return false;
}

LTSTATUS::LTSTATUS TRDPkgHeader::IsHeaderPresent(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) {
    TRDPkgHeader hdr(fStreamInfo);
    TRD_HEADER h;
    bool status = hdr.ReadHeaderBack(h);
    LTSTATUS::LTSTATUS errCode = hdr.e.GetError();
    if (!status || errCode != LTSTATUS::SUCCESS || hdr.ValidateHeader(h) != LTSTATUS::SUCCESS) {
        dbgprintf("header not present\n");
        return errCode;
    }
    
    return LTSTATUS::SUCCESS;

}



LTSTATUS::LTSTATUS TRDPkgHeader::ValidateHeader(const TRD_HEADER& hdrIn) {
    if (!ICheckHeaderSize(hdrIn)) {
        return LTSTATUS::HDRNP;
    }
    if (!std::equal(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdrIn.magic))) {
        return LTSTATUS::HDRNP;
    }
    if (hdrIn.exSignature != TRD_HDR_EXTENDED_SIGNATURE) {
        return LTSTATUS::HDRCRP;
    }
    if (hdrIn.fmtVersion == 0) {
        return LTSTATUS::BADARG;
    }
    if (!ICheckCRC(hdrIn.hdrChksum, hdrIn)) {
        return LTSTATUS::CHKSUM;
    }
    if (hdrIn.fileLen == 0) {
        return LTSTATUS::BADARG;
    }
    //todo check each field including signature
    return LTSTATUS::OK;
}

bool TRDPkgHeader::WriteHeader() {
    if (!ICheckHeaderSize(hdrInteral)) {
        e.SetError(LTSTATUS::HDRCRP);
        return false;
    }
    if (!IsValid()) {
        return false;
    }
    auto [checkWeakRef, sharedPtr] = FstreamInfo::TrdFstreamInfo::GetFstreamContent(wFstr);
    if (!checkWeakRef) {
        e.SetError(LTSTATUS::IREF_EXPIRED);
        return false;
    }
    if (!sharedPtr->CheckFileStreamInfo()) {
        e.SetError(LTSTATUS::NULL_OBJ);
        return false;
    }
    auto& fstrInfo = sharedPtr->GetFstreamObject();
    auto& fstrStream = fstrInfo.hFile;


    const char* hdrContent = reinterpret_cast<const char*>(&hdrInteral);
  //  streamInfo->hFile->WriteHeader(hdrContent, GetHeaderByteSize());
    fstrStream->seekp(TRD_HDR_START_OFFSET);
    if (!fstrStream) {
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    bool WriteHeaderStatus = PkgIO::FileOperations::WriteLeStream(fstrInfo, hdrContent, LT_HDR_SZB_01A, false);
   
    if (!fstrStream || !WriteHeaderStatus) {
        e.SetError(LTSTATUS::IO_WRITE);
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
//
#define _LOCAL_CRC(crc,x) crc32(((crc)), reinterpret_cast<const Bytef*>(&(x)), sizeof((x)))
u32 TRDPkgHeader::IGenerateHeaderCRC(const TRD_HEADER& hdr) {
    u32 crc = crc32(0, Z_NULL, 0);
    crc = _LOCAL_CRC(crc, hdr.magic);
    crc = _LOCAL_CRC(crc, hdr.exSignature);
    crc = _LOCAL_CRC(crc, hdr.fmtVersion);
    crc = _LOCAL_CRC(crc, hdr.compression);
    crc = _LOCAL_CRC(crc, hdr.buildFlags);
    crc = _LOCAL_CRC(crc, hdr.architecture);
    return crc;
}
bool TRDPkgHeader::ICheckCRC(u32 crc, const TRD_HEADER& hdr) {
    u32 newCrc = IGenerateHeaderCRC(hdr);
    return newCrc == crc;
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
        e.SetError(LTSTATUS::COPYOBJ);
        return false;
    }
    hdr.exSignature = TRD_HDR_EXTENDED_SIGNATURE;
    hdr.fmtVersion = field.fmtVersion;
    if (hdr.fmtVersion == 0) {
        e.SetError(LTSTATUS::BADARG);
        return false;
    }
    hdr.compression = field.compression;
    hdr.buildFlags = field.buildFlags;
    hdr.architecture = field.architecture;
    hdr.hdrChksum = IGenerateHeaderCRC(hdr);
    
    hdr.fileLen = UINT64_MAX;
    hdr.ioCtrl = IOCTRL_CLEAR;

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
    hdr.architecture = update.architecture;
    hdr.fmtVersion = update.fmtVersion;
    hdr.compression = update.compression;
    hdr.buildFlags = update.buildFlags;
    hdr.hdrChksum = IGenerateHeaderCRC(hdr);

    LTSTATUS::LTSTATUS err = ValidateHeader(hdr);
    if (err != LTSTATUS::SUCCESS) {
        e.SetError(err);
        return false;
    } 
    hdrInteral = hdr;
    e.Success();
    
    return WriteHeader();
 }

 bool TRDPkgHeader::SetIoctrl(u16 ioctrl, bool autoWrite) {
    //todo check if valid
    hdrInteral.ioCtrl = ioctrl;
    e.Success();
    
    bool status = true;
    if (autoWrite) {
        status = WriteHeader();
    }
    
    return status;
}

 bool TRDPkgHeader::SetFileLen(u64 len, bool autoWrite) {
    //todo check if valid
    hdrInteral.fileLen = len;
    e.Success();

    bool status = true;
    if (autoWrite) {
        status = WriteHeader();
    }
    
    return status;
 }