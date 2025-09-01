#include "trheader.h"
#include <sstream>
using namespace LibTrident;
using namespace LibTrident::Header;

#define LT_HDR_SZB_01A 32
//01.0.0
#define LT_HDR_VERSION_MIN 1000

bool PackageHeader::Sync(TRD_HEADER& out) {
    return true;
}

bool PackageHeader::CheckHeaderSize(TRD_HEADER hdr) {
    if (sizeof(hdr) != LT_HDR_SZB_01A) {
        return false;
    }
    return true;
}
bool PackageHeader::CreateNewHeader(TRD_HEADER& hdrOut, u32 buildFlgs, u8 archType, u8 comprType) {
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

u16 PackageHeader::FormatHeaderVersion(u8 major, u8 minor, u8 revision) {
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
std::string PackageHeader::HeaderVersionFormatToString(u16 fmt, bool abRevision) {
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