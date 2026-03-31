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

struct TRD_HDRFIELD_UPDATE{
    std::optional<u16> fmtVersion;
    std::optional<GlobalCompression::Algorithm> compression;
    std::optional<BuildFlags::Flags> buildFlags;
    std::optional<ArchType::Type> architecture;
};
/// TRPX file header
struct TRD_HEADER : Impl::SerializableData {
    //we are not using byte or unsigned char as ive read that its somehow not well standardized
    //and on different compilers we could get different results
   
     //why simply not use const here ? using const prevents struct assigning as const
    //cannot be assigned
    std::array<u8,8> magic = std::to_array(Consts::Header::TRD_HDR_MAGIC);
    u16 exSignature = Consts::Header::TRD_HDR_EXTENDED_SIGNATURE;
    u16 fmtVersion = Consts::Header::TRD_HDR_INVALID_VERSION;
    
    GlobalCompression::Algorithm compression = GlobalCompression::None;
    BuildFlags::Flags buildFlags = BuildFlags::Clear;
    ArchType::Type architecture = ArchType::Any; 
    u32 dynHdrChksum = Consts::TRD_INVALID_CHKSUM;
    u64 dynFileLen;
    u16 _reserved0;

    //PackageIOCtrl::Flag dynIoCtrl = PackageIOCtrl::Clear; //moved to SD

    constexpr u64 size() const override {
        return Impl::BinarySerializer::elementSize(magic, exSignature,
        fmtVersion, compression, buildFlags, architecture, dynHdrChksum, dynFileLen, _reserved0);
    }

    std::optional<std::vector<u8>> serialize() const override {
        if (_reserved0 != 0) { 
            //todo check here the fields that should be const like header so we dont have to check manually
            //in code always, as the header field only matters when doing CRC, serialization and deserialization
            return std::nullopt;
        }
        Trd::Impl::BinarySerializer bs;
        
    
        bs.addContainer(std::span<const u8>(magic));
        bs.addTrivial(exSignature,fmtVersion,compression, buildFlags,
                        architecture, dynHdrChksum, dynFileLen, _reserved0);
        
        
        return bs.getFormattedData();

    }

    
    bool deserialize(const std::vector<u8>& dataIn) override {
        Impl::BinarySerializer bs(dataIn);
        
        bs.readContainer(std::span<u8>(magic));
      
        bs.readTrivial(exSignature, fmtVersion,compression,
                                    buildFlags, architecture, dynHdrChksum,
                                    dynFileLen, _reserved0);
       
        dbgprintf("xsize:%ld:\n", bs.getReadOffset());
        if (bs.getReadOffset() != this->size() || _reserved0 != 0){
            return false;
        }
        return true;
    }
    std::optional<u32> checksumCRC32() const override {
        if (_reserved0 != 0) {
            return std::nullopt;
        }
        
        Impl::Crc32Gen crc;
        crc.addData(magic, exSignature,fmtVersion,
                        compression,buildFlags,architecture, _reserved0);
        return crc.getCrc32();
    }
    //todo implement recalcCRC()
};




};