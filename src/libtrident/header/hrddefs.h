#pragma once
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"
#include <span>
namespace LibTrident {
/*
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
*/
struct TRD_HEADER : PkgIO::SerializableData {
    //we are not using byte or unsigned char as ive read that its somehow not well standardized
    //and on different compilers we could get different results
   
    std::array<u8,8> magic = std::to_array(LibTrident::Consts::HeaderConsts::TRD_HDR_MAGIC);
    u16 exSignature = LibTrident::Consts::HeaderConsts::TRD_HDR_EXTENDED_SIGNATURE;
    u16 fmtVersion;
    //todo use enum classes for supported dt's and convert it in serialize/deserialize
    u8 compression;
    u32 buildFlags;
    u8 architecture; 
    u32 dynHdrChksum;
    u64 dynFileLen;
    u16 dynIoCtrl;

    constexpr size_t size() const override {
        return PkgIO::BinarySerializer::elementSize(magic, exSignature,
        fmtVersion, compression, buildFlags, architecture, dynHdrChksum, dynFileLen, dynIoCtrl);
    }
    std::optional<std::vector<u8>> serialize() const override {
        LibTrident::PkgIO::BinarySerializer bs;
       
    
        bs.addContainer(std::span<const u8>(magic));
        bs.addTrivial(exSignature,fmtVersion,compression, buildFlags,
                        architecture, dynHdrChksum, dynFileLen, dynIoCtrl);
        
        return bs.getFormattedData();

    }

    
    bool deserialize(const std::vector<u8>& dataIn) override {
        PkgIO::BinarySerializer bs(dataIn);
        size_t xsize = 0;
       
        xsize += bs.readContainer(std::span<u8>(magic), xsize);
        xsize += bs.readTrivial<u16>(&exSignature, xsize);
        xsize += bs.readTrivial<u16>(&fmtVersion, xsize);
        xsize += bs.readTrivial<u8>(&compression, xsize);
        xsize += bs.readTrivial<u32>(&buildFlags, xsize);
        xsize += bs.readTrivial<u8>(&architecture, xsize);
        xsize += bs.readTrivial<u32>(&dynHdrChksum, xsize);
        xsize += bs.readTrivial<u64>(&dynFileLen, xsize);
        xsize += bs.readTrivial<u16>(&dynIoCtrl, xsize);
        
        
        if (xsize != this->size()){
            return false;
        }
        return true;
    }
    std::optional<u32> checksumCRC32() const override {
        PkgIO::Crc32Gen crc;
        crc.addData(magic, exSignature,fmtVersion,
                        compression,buildFlags,architecture);
        return crc.getCrc32();
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