#pragma once

#include "trderr.h"
#include "datatypes.h"
#include "ccattribs.h"
#include "fstreaminfo.h"
namespace LibTrident::Header {

PACKED_STRUCT {
    byte magic[8];
    u16 exSignature;
    u16 fmtVersion;
    u8 compression;
    u32 buildFlags;
    u8 architecture;
    u32 hdrChksum;
    u64 fileLen;
    u16 ioCtrl;

}TRD_HEADER;
//when printing dont forget to add NULL terminator
constexpr byte TRD_HDR_MAGIC[] = {
    0x93, 0x54, 0x52, 0x44, 0x21, 0x12, 0x2E, 0x53
};//\223TRD!\x12.S
constexpr u16 HDR_EXTENDED_SIGNATURE = 0xbf97;
constexpr u8 HDR_VMAJOR = 1;
constexpr u8 HDR_VMINOR = 0;
constexpr u8 HDR_VREVISION = 0;

typedef enum {
    COMMPRALG_NONE,
    COMPRALG_LZ4,
    COMPRALG_GZIP
}COMPR_ALGO;

typedef enum {
    BF_CLEAR,
    BF_PLATF_LINUX = 1 << 1,
    BF_PLATF_NT = 1 << 2,
    BF_DEBUG = 1 << 3,
   


}BUILD_FLAGS;

typedef enum {
    ARCHF_ANY = 0,
    ARCHT_AM64 = 1 << 1,
    ARCHT_I386 = 1 << 2,
    ARCHT_AARCH64 = 1 << 3
}ARCH_TYPES;

typedef enum {
    IOCTRL_CLEAR = 0,
    IOCTRL_READLKALL = 1 << 1,
    IOCTRL_WRITELKKALL = 1 << 2,
    IOCTRL_DSEC_RLOCK = 1 << 3,
    IOCTRL_DESC_WLOCK = 1 << 4,
    IOCTRL_TREG_RLOCK = 1 << 5,
    IOCTRL_TREG_WLOCK = 1 << 6,

    IOCTRL_LOCKALL = 1 << 16
}PKG_IOCTRL;




class PackageHeader  {
public:
    PackageHeader(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) :
    fstrInfo(fStreamInfo) {}
    PackageHeader(const PackageHeader& other) : fstrInfo(other.fstrInfo) {}
    PackageHeader(PackageHeader&& other) : fstrInfo(std::move(other.fstrInfo)) {}
    

    LTSTATUS::TridentError e;
    bool CreateNewHeader(TRD_HEADER& hdrOut, u32 buildFlgs, u8 archType, u8 comprType = COMMPRALG_NONE);
    bool WriteHeader(TRD_HEADER& hdrIn);
    bool ReadHeader(TRD_HEADER& hdrOut);
    bool UpdateHeader(TRD_HEADER& hdrInfo);
    //Flushes info to the file without closing FD
    bool Sync(TRD_HEADER& hdrOut);
    bool ValidateHeader(TRD_HEADER& hdrIn);
    static u16 FormatHeaderVersion(u8 major, u8 minor, u8 revision);
    static std::string HeaderVersionFormatToString(u16 fmt, bool abRevision=true);

  
private:
std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fstrInfo;
//TRD_HEADER cacheHdr;

static bool ICheckHeaderSize(const TRD_HEADER& hdr);
std::pair<bool,std::shared_ptr<FstreamInfo::TRDFilStreameInfo>> ICheckAndGetFstreamContent();
bool IHeaderPresent();
//static u16 FormatHeaderVersion(u8 major, u8 minor, u8 revision);

};
};