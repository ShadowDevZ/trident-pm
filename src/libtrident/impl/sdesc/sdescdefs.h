#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"

namespace Trd::Impl {

typedef struct {
    u32 tblCount;
    u64 tblDynamicOffset;
    u64 tblRegistryOffset;
}TRD_SD_UPDATEFIELD;

struct SectionDesriptor : Impl::SerializableData {
    u32 crc; 
    u32 tblCount;
    u64 tblDynamicOffset;
    u64 tblRegistryOffset;
    u64 _reserved0;

    constexpr u64 size() const override {
        return Impl::BinarySerializer::elementSize(crc, tblCount, tblDynamicOffset, 
                                                  tblRegistryOffset, _reserved0);
    }
    std::optional<std::vector<u8>> serialize() const override {
        Trd::Impl::BinarySerializer bs;
        bs.addTrivial(crc, tblCount, tblDynamicOffset, tblRegistryOffset, _reserved0);
        return bs.getFormattedData();
    }
    bool deserialize(const std::vector<u8>& dataIn) override {
        Impl::BinarySerializer bs(dataIn);
        bs.readTrivial(crc, tblCount, tblDynamicOffset, tblRegistryOffset, _reserved0);
        dbgprintf("xsize:%ld:\n", bs.getReadOffset());
        if (bs.getReadOffset() != this->size() || _reserved0 != 0){
            return false;
        }
        return true;
    }   
    std::optional<u32> checksumCRC32() const override {
        Impl::Crc32Gen crc;
        crc.addData(tblCount, tblDynamicOffset, tblRegistryOffset, _reserved0);
        return crc.getCrc32();
    }


};
};