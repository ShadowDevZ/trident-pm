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

typedef struct {
    u16 fmtVersion;
    u8 compression;
    u32 buildFlags;
    u8 architecture;
}TRD_HDRFIELD_UPDATE;

//when printing dont forget to add NULL terminator
constexpr byte TRD_HDR_MAGIC[] = {
    0x93, 0x54, 0x52, 0x44, 0x21, 0x12, 0x2E, 0x53
};//\223TRD!\x12.S
constexpr u16 TRD_HDR_EXTENDED_SIGNATURE = 0xbf97;
constexpr u8 TRD_HDR_VMAJOR = 1;
constexpr u8 TRD_HDR_VMINOR = 0;
constexpr u8 TRD_HDR_VREVISION = 0;


constexpr u32 HDR_START_OFFSET = 0;
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
    IOCTRL_ReadHeaderLKALL = 1 << 1,
    IOCTRL_WriteHeaderLKKALL = 1 << 2,
    IOCTRL_DSEC_RLOCK = 1 << 3,
    IOCTRL_DESC_WLOCK = 1 << 4,
    IOCTRL_TREG_RLOCK = 1 << 5,
    IOCTRL_TREG_WLOCK = 1 << 6,

    IOCTRL_LOCKALL = 1 << 16
}PKG_IOCTRL;




class TRDPkgHeader  {
public:
    TRDPkgHeader(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDPkgHeader(const TRDPkgHeader& other) : wFstr(other.wFstr) {}
    TRDPkgHeader(TRDPkgHeader&& other) : wFstr(std::move(other.wFstr)) {}
    

    LTSTATUS::TridentError e;
 

    const TRD_HEADER& GetInternal() const { 
        return hdrInteral;
    }
    bool Create(u32 buildFlgs, u8 archType, u8 comprType = COMMPRALG_NONE);
    bool Create(const TRD_HDRFIELD_UPDATE& field);
    bool WriteHeader();
    bool ReadHeader();
    bool ReadHeaderBack(TRD_HEADER& hdrOut);

    
    bool UpdateHeader(const TRD_HDRFIELD_UPDATE& update);
    bool SetIoctrl(u16 ioctrl, bool autoWrite=true);
    bool SetFileLen(u64 len, bool autoWrite=true);
   
    
    bool IsValid();
    static LTSTATUS::LTSTATUS ValidateHeader(const TRD_HEADER& hdrIn);
    static u16 FormatHeaderVersion(u8 major, u8 minor, u8 revision);
    static std::string HeaderVersionFormatToString(u16 fmt, bool abRevision=true);
    
    static LTSTATUS::LTSTATUS IsHeaderPresent(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo);
    static int GetHeaderByteSize();
    
  
private:
std::weak_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> wFstr;
TRD_HEADER hdrInteral;
//TRD_HEADER cacheHdr;
static u32 IGenerateHeaderCRC(const TRD_HEADER& hdr);
static bool ICheckCRC(u32 crc, const TRD_HEADER& hdr);
static bool ICheckHeaderSize(const TRD_HEADER& hdr);
//std::pair<bool,FstreamInfo::TRDFstreamObject&> ICheckAndGetFstreamContent();


//static u16 FormatHeaderVersion(u8 major, u8 minor, u8 revision);

};
};