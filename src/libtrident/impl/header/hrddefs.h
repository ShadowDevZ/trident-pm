/**
 * @file hrddefs.h
 * @brief Header section definitions
 * 
 * 
 */
#pragma once
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"
#include <span>
namespace Trd {

/**
 * @brief Compression algorithm referenced by data and tables
 * 
 */
namespace GlobalCompression {
    enum Algorithm : u8 {
        None,
        LZ4,
        GZip
    };
};
/**
 * @brief Flags t odetermine how the package was built for quick checking
 * 
 */
namespace BuildFlags {
    enum Flags : u32 {
        Clear,
        PlatformLinux = 1 << 1,
        PlatformNT = 1 << 2,
        PlatformyAny = 1 << 3,
        Debug = 1 << 4,
        DebugWithSymbols = 1 << 5
    };
};
/**
 * @brief Type of architecture which is the package supposed to run on
 * 
 */
namespace ArchType {
    enum Type : u8{
        Any = 0,
        Amd64 = 1 << 1,
        X86 = 1 << 2,
        Aarch64 = 1 << 3
    };
};
/**
 * @brief Internal locking flags
 * @details Provides flags for locking certain section,tables or parts of file when
 * working with multiple instances. For example instance 1 is accessing the header for reading
 * whilst instance 2 is trying to read SD, this makes the work much faster when dealing with huge
 * packages. However each operation is locked internally so we dont have race condition when modifying data
 * so only read access is provided. This field is hint and in no way is there to physically
 * lock the package as this warning can freely be bypassed. This also fixes the problems if instance 1
 * is writing header as instance 2 tries to read it in this case this section is write protected.
 * 
 */
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
/// TRPX file header
struct TRD_HEADER : Impl::SerializableData {
    //we are not using byte or unsigned char as ive read that its somehow not well standardized
    //and on different compilers we could get different results
   
    std::array<u8,8> magic = std::to_array(Consts::Header::TRD_HDR_MAGIC);
    u16 exSignature = Consts::Header::TRD_HDR_EXTENDED_SIGNATURE;
    u16 fmtVersion = Consts::Header::TRD_HDR_INVALID_VERSION;
    
    GlobalCompression::Algorithm compression = GlobalCompression::None;
    BuildFlags::Flags buildFlags = BuildFlags::Clear;
    ArchType::Type architecture = ArchType::Any; 
    u32 dynHdrChksum = Consts::Header::TRD_HDR_INVALID_CHKSUM;
    u64 dynFileLen;
    PackageIOCtrl::Flag dynIoCtrl = PackageIOCtrl::Clear;

    constexpr size_t size() const override {
        return Impl::BinarySerializer::elementSize(magic, exSignature,
        fmtVersion, compression, buildFlags, architecture, dynHdrChksum, dynFileLen, dynIoCtrl);
    }

    std::optional<std::vector<u8>> serialize() const override {
        Trd::Impl::BinarySerializer bs;
       
    
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