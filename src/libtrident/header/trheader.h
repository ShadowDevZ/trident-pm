#pragma once

#include "trderr.h"
#include "datatypes.h"
#include "ccattribs.h"
#include "tstreaminfo.h"
#include "trdconsts.h"
#include "sectioncommon.h"
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


//todo each SECTION should inherit from something like SectionCommon, standardize the functions
class TRDPkgHeader : public LibTrident::Sections::SectionCommon<TRD_HEADER>  {
public:
    TRDPkgHeader(std::shared_ptr<LibTrident::Tstream::TStreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDPkgHeader(const TRDPkgHeader& other) : wFstr(other.wFstr) {}
    TRDPkgHeader(TRDPkgHeader&& other) : wFstr(std::move(other.wFstr)) {}
    

    LibTrident::Err::TridentError e;
 

    const TRD_HEADER& GetObject() const override { 
        return hdrInteral;
    }
    TRD_HEADER& GetObject() override{ 
        return hdrInteral;
    }
    inline bool StatusOk() override {
        return e.IsOk();
    }

    bool Write() override;
    bool Read() override;
    std::optional<TRD_HEADER> ReadBack() override;
    bool IsValid() override;
    
    
    bool Create(u32 buildFlgs, u8 archType, u8 comprType = COMMPRALG_NONE);
    bool UpdateHeader(const TRD_HDRFIELD_UPDATE& update);
    bool Create(const TRD_HDRFIELD_UPDATE& field);
    bool UpdateIoctrlProp(u16 ioctrl);
    bool UpdateFileLenProp(u64 len);
    
    
    static u16 FormatHeaderVersion(u8 major, u8 minor, u8 revision);
    static std::string HeaderVersionFormatToString(u16 fmt, bool abRevision=true);
    
    static LibTrident::Err::Code IsHeaderPresent(std::weak_ptr<LibTrident::Tstream::TStreamInfo> fStreamInfo);
    
    
    
    private:
    std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr;
    TRD_HEADER hdrInteral;
    //TRD_HEADER cacheHdr;
   
    bool ICheckCRC(u32 crc, const TRD_HEADER& hdr);
    bool ICheckHeaderSize(const TRD_HEADER& hdr);
    bool IValidateHeader(const TRD_HEADER& hdrIn);
    //std::pair<bool,Tstream::TRDFstreamObject&> ICheckAndGetFstreamContent();
    
    
    //static u16 FormatHeaderVersion(u8 major, u8 minor, u8 revision);
    
};
};