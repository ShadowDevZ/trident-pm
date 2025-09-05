#include "trheader.h"
#include <sstream>

using namespace LibTrident;
using namespace LibTrident::Header;



#define LT_HDR_SZB_01A 32
//01.0.0
#define LT_HDR_VERSION_MIN 1000
std::pair<bool,std::shared_ptr<FstreamInfo::TRDFilStreameInfo>> TRDPkgHeader::ICheckAndGetFstreamContent() {
    if (!fstrInfo->CheckFileStreamInfo()) {
        e.SetError(fstrInfo->e);
        
        
        return {false, nullptr};
    }
    e.Success();
    return {true, fstrInfo->GetFileStreamInfo()};
}



constexpr int GetHeaderByteSize() {
    return LT_HDR_SZB_01A;
}


bool TRDPkgHeader::ReadHeader(TRD_HEADER& hdrOut) {
    TRD_HEADER hdr = {};
    
    hdrOut = hdr;
    auto [checkFstream, streamPtr] = ICheckAndGetFstreamContent();
    if (!checkFstream || streamPtr == nullptr) {
        e.SetError(LTSTATUS::NULL_OBJ);
        return false;
    }
    std::fstream& readStream = *streamPtr->hFile;
    
    std::streampos originalPosition = readStream.tellg();
    readStream.seekg(0, std::ios::beg);
    if (readStream.fail()) {
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    if (!ICheckHeaderSize(hdr)) {
        e.SetError(LTSTATUS::HDRCRP);
        return false;
    }
    
    readStream.read(reinterpret_cast<char*>(&hdr), GetHeaderByteSize());
    if (!readStream) {
        e.SetError(LTSTATUS::IOREAD);
        return false;
    }

    readStream.seekg(originalPosition, std::ios::beg);
    if (readStream.fail()) {
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    if (!ValidateHeader(hdr)) {
        return false;
    }
    e.Success();
    hdrOut = hdr;
    return true;
}
bool TRDPkgHeader::IsWrittenHeaderValid() {
    TRD_HEADER hdr = {};
    bool status = ReadHeader(hdr);
    if (!status) {
        return false;
    }
    return ValidateHeader(hdr);

}

bool TRDPkgHeader::ValidateHeader(TRD_HEADER& hdrIn) {
    if (!ICheckHeaderSize(hdrIn)) {
        e.SetError(LTSTATUS::HDRNP);
        return false;
    }
    if (!std::equal(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdrIn.magic))) {
        e.SetError(LTSTATUS::HDRCRP);
        return false;
    }
    if (hdrIn.exSignature != HDR_EXTENDED_SIGNATURE) {
        return false;
    }
    //todo check each field including signature
    return true;
}

bool TRDPkgHeader::WriteHeader(TRD_HEADER& hdrIn) {
    if (!ICheckHeaderSize(hdrIn)) {
        e.SetError(LTSTATUS::HDRCRP);
        return false;
    }

    auto [checkFstream, writeStream] = ICheckAndGetFstreamContent();
    if (!checkFstream || writeStream == nullptr) {
        return false;
    }
    if (!ValidateHeader(hdrIn)) {
        return false;
    }
    auto hdrContent = reinterpret_cast<const char*>(&hdrIn);
    writeStream->hFile->write(hdrContent, GetHeaderByteSize());
    if (!writeStream->hFile) {
        e.SetError(LTSTATUS::IOWRITE);
        return false;
    }
    

    return true;
}


bool TRDPkgHeader::ICheckHeaderSize(const TRD_HEADER& hdr) {
    if (sizeof(hdr) != GetHeaderByteSize()) {
        return false;
    }
    return true;
}
bool TRDPkgHeader::CreateNewHeader(TRD_HEADER& hdrOut, u32 buildFlgs, u8 archType, u8 comprType) {
    TRD_HEADER hdr;
    std::copy(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdr.magic));
    if (!std::equal(std::begin(TRD_HDR_MAGIC), std::end(TRD_HDR_MAGIC), std::begin(hdr.magic))) {
        e.SetError(LTSTATUS::COPYOBJ);
        return false;
    }
    hdr.exSignature = HDR_EXTENDED_SIGNATURE;
    hdr.fmtVersion = FormatHeaderVersion(HDR_VMAJOR, HDR_VREVISION, HDR_VREVISION);
    if (hdr.fmtVersion == 0) {
        return false;
    }
    hdr.compression = comprType;
    hdr.buildFlags = buildFlgs;
    hdr.architecture = archType;
    hdr.hdrChksum = 0xBEEF;
    
    hdr.fileLen = UINT64_MAX;
    hdr.ioCtrl = IOCTRL_CLEAR;
    hdrOut = hdr;
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
    //the r==0 is there only for readability because strtoul may return 0 on failure
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