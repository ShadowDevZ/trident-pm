#pragma once
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"
#include <span>
namespace LibTrident {


namespace GlobalCompression {
    enum Algorithm : u8 {
        None,
        LZ4,
        GZip
    };
};
namespace BuildFlags {
    enum Flags : u32 {
        Clear,
        PlatformLinux = 1 << 1,
        PlatformNT = 1 << 2,
        Debug = 1 << 3,
        DebugWithSymbols = 1 << 4
    };
};
namespace ArchType {
    enum Type : u8{
        Any = 0,
        Amd64 = 1 << 1,
        X86 = 1 << 2,
        Aarch64 = 1 << 3
    };
};
namespace PackageIOCtrl {
    enum Flag : u16 {
        Clear = 0,
        ReadHeaderLock = 1 << 1,
        WriteHeaderLock = 1 << 2,
        DynamicSectionReadLock = 1 << 3,
        DynamicSectionWriteLock = 1 << 4,
        TregReadLock = 1 << 5,
        TregWriteLock = 1 << 6,

        LockAll = 1 << 15
};
};
struct TRD_HDRFIELD_UPDATE{
    u16 fmtVersion;
    GlobalCompression::Algorithm compression;
    BuildFlags::Flags buildFlags;
    ArchType::Type architecture;
};

struct TRD_HEADER : Impl::SerializableData {
    //we are not using byte or unsigned char as ive read that its somehow not well standardized
    //and on different compilers we could get different results
   
    std::array<u8,8> magic = std::to_array(Consts::HeaderConsts::TRD_HDR_MAGIC);
    u16 exSignature = Consts::HeaderConsts::TRD_HDR_EXTENDED_SIGNATURE;
    u16 fmtVersion = Consts::HeaderConsts::TRD_HDR_INVALID_VERSION;
    //todo use enum classes for supported dt's and convert it in serialize/deserialize
    GlobalCompression::Algorithm compression = GlobalCompression::None;
    BuildFlags::Flags buildFlags = BuildFlags::Clear;
    ArchType::Type architecture = ArchType::Any; 
    u32 dynHdrChksum;
    u64 dynFileLen;
    PackageIOCtrl::Flag dynIoCtrl = PackageIOCtrl::Clear;

    constexpr size_t size() const override {
        return Impl::BinarySerializer::elementSize(magic, exSignature,
        fmtVersion, compression, buildFlags, architecture, dynHdrChksum, dynFileLen, dynIoCtrl);
    }

    std::optional<std::vector<u8>> serialize() const override {
        LibTrident::Impl::BinarySerializer bs;
       
    
        bs.addContainer(std::span<const u8>(magic));
        bs.addTrivial(exSignature,fmtVersion,compression, buildFlags,
                        architecture, dynHdrChksum, dynFileLen, dynIoCtrl);
        
        return bs.getFormattedData();

    }

    
    bool deserialize(const std::vector<u8>& dataIn) override {
        Impl::BinarySerializer bs(dataIn);
        size_t xsize = 0;
       
        xsize += bs.readContainer(std::span<u8>(magic), xsize);
      
        xsize += bs.readTrivial(xsize, exSignature, fmtVersion,compression,
                                    buildFlags, architecture, dynHdrChksum,
                                    dynFileLen, dynIoCtrl);
       
        dbgprintf("xsize:%ld:\n", xsize);
        if (xsize != this->size()){
            return false;
        }
        return true;
    }
    std::optional<u32> checksumCRC32() const override {
        Impl::Crc32Gen crc;
        crc.addData(magic, exSignature,fmtVersion,
                        compression,buildFlags,architecture);
        return crc.getCrc32();
    }

};




};