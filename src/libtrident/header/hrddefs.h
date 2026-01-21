#pragma once
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "pkgio.h"
namespace LibTrident {

struct __attribute__((packed)) TRD_HEADER{
    byte magic[8];
    u16 exSignature;
    u16 fmtVersion;
    u8 compression;
    u32 buildFlags;
    u8 architecture;
    u32 hdrChksum;
    u64 fileLen;
    u16 ioCtrl;

};

struct XTRD_HEADER : PkgIO::SerializableData {
    //we are not using byte or unsigned char as ive read that its somehow not well standardized
    //and on different compilers we could get different results
    //u8 magic[8];
    //u64 magic;
    std::array<u8,8> magic {{0x93, 0x54, 0x52, 0x44, 0x21, 0x12, 0x2E, 0x53}};
    u16 exSignature;
    u16 fmtVersion;
    u8 compression;
    u32 buildFlags;
    u8 architecture;
    u32 hdrChksum;
    u64 fileLen;
    u16 ioCtrl;

    size_t size() const override {
        return PkgIO::BinarySerializer::elementSize(magic, exSignature,
        fmtVersion, compression, buildFlags, architecture, hdrChksum, fileLen, ioCtrl);
    }
    std::optional<std::vector<u8>> serialize() const override {
        LibTrident::PkgIO::BinarySerializer bs;
        // bs.addTrivial(magic);
        bs.addTrivial(exSignature);
        bs.addType(magic);
        bs.addTrivial(fmtVersion);
        bs.addTrivial(compression);
        bs.addTrivial(buildFlags);
        bs.addTrivial(architecture);
        bs.addTrivial(hdrChksum);
        bs.addTrivial(fileLen);
        bs.addTrivial(ioCtrl);
        
        return bs.getFormattedData();

    }

    
    bool deserialize(const std::vector<u8>& dataIn) override {
        PkgIO::BinarySerializer bs(dataIn);
        size_t xsize = 0;
       
        xsize += bs.readTrivial<u16>(&exSignature, xsize);
        xsize += bs.readType(std::span<u8>(magic), xsize);
        xsize += bs.readTrivial<u16>(&fmtVersion, xsize);
        xsize += bs.readTrivial<u8>(&compression, xsize);
        xsize += bs.readTrivial<u32>(&buildFlags, xsize);
        xsize += bs.readTrivial<u8>(&architecture, xsize);
        xsize += bs.readTrivial<u32>(&hdrChksum, xsize);
        xsize += bs.readTrivial<u64>(&fileLen, xsize);
        xsize += bs.readTrivial<u16>(&ioCtrl, xsize);
        
        
        if (xsize != this->size()){
            return false;
        }
      
       // xsize += bs.ReadRaw(&x, sizeof(x), xsize);
      //  xsize += bs.ReadRaw(&y, sizeof(y), xsize);
        //xsize += bs.ReadRaw(&z, sizeof(z), xsize);
        return true;
    }

};

struct TRD_HDRFIELD_UPDATE{
    u16 fmtVersion;
    u8 compression;
    u32 buildFlags;
    u8 architecture;
};


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

};