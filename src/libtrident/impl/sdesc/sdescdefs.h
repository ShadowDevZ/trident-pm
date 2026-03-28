#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"



namespace Trd::Impl {
/*allows smooth control of multiple different processes accessing the same file resource
and the same part of library checking. Note if status flag is not clear then the 
other process MUST NOT perform any IO operation which could
alter the file manipulation by the host program under any circumstances, this check should be done
using some OS specific function by other process accessing this resource not as stored field in case
of crash the package would be bricked 
permamently*/
namespace SectionStatusFlag {
    enum Flag : u16 {
        Clear = 0,
        ReadLockHeader = 1 << 1, //header write operation in progress reading not advised
        WriteLockHeader = 1 << 2,

        ReadLockDynamic = 1 << 3,
        WriteLockDynamic = 1 << 4,
        
        ReadLockTreg = 1 << 5,
        WriteLocTreg = 1 << 6,

        LockAll = 1 << 15
};
};


typedef struct {
    u32 tblCount;
    u64 tblDynamicOffset;
    u64 tblRegistryOffset;
}TRD_SD_UPDATEFIELD;

struct TRD_SECTION_DESCRIPTOR : Impl::SerializableData {
    u32 crc;
    /*status code bitflags to determine what part of offsets contain valid offset
    for example if dynamic offset flag is cleared and some resizing of .dtbl occurs
    because we are adding a new table. */
    u16 sectionStatusCode; 
  

    u32 tblCount; 
    u64 tblDynamicOffset;
    u64 tblRegistryOffset;
    u64 _reserved0;

    constexpr u64 size() const override {
        return Impl::BinarySerializer::elementSize(crc, tblCount, tblDynamicOffset, 
                                                  tblRegistryOffset, _reserved0);
    }
    std::optional<std::vector<u8>> serialize() const override {
        if (_reserved0 != 0) {
            return std::nullopt;
        }
        Trd::Impl::BinarySerializer bs;
        bs.addTrivial(crc, tblCount, tblDynamicOffset, tblRegistryOffset, _reserved0);
        return bs.getFormattedData();
    }
    bool deserialize(const std::vector<u8>& dataIn) override {
        if (_reserved0 != 0) {
            return false;
        }
        Impl::BinarySerializer bs(dataIn);
        bs.readTrivial(crc, tblCount, tblDynamicOffset, tblRegistryOffset, _reserved0);
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
        crc.addData(tblCount, tblDynamicOffset, tblRegistryOffset, _reserved0);
        return crc.getCrc32();
    }


};
};