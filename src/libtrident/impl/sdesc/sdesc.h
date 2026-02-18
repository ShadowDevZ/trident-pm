#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "ccattribs.h"
#include "tstreaminfo.h"
#include "binarySerializer.h"
#include "sectioncommon.h"
namespace Trd::Impl {

PACKED_STRUCT {
    u32 crc; 
    u32 tblCount;
    u64 tblDynamicOffset;
    u64 tblRegistryOffset;
    u64 _reserved0;
}TRD_SD;

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




class TRDSecDesc : public Trd::Sections::SectionCommon<TRD_SD> {
private:
    std::weak_ptr<Trd::Impl::TStreamInfo> wFstr;
    TRD_SD secDescInternal;

public:
   
    TRDSecDesc(std::shared_ptr<Trd::Impl::TStreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDSecDesc(const TRDSecDesc& other) : wFstr(other.wFstr) {}
    TRDSecDesc(TRDSecDesc&& other) : wFstr(std::move(other.wFstr)) {}
    
    const TRD_SD& GetObject() const noexcept override { 
        return secDescInternal;
    }
    TRD_SD& GetObject() noexcept override { 
        return secDescInternal;
    }
    std::expected<void, Err::TrdError> Write() override {
        return IWriteSD(false);
    }
   
    bool IsValid() override;
    std::expected<void, Err::TrdError> Read() override;
    std::expected<TRD_SD, Err::TrdError> ReadBack() override;
   //creates blank section
   //we cant really create it like header because there is 0 initial information to append during sequentional 
   //initialization as there are 0 tables
    
   //Create()
    std::expected<void, Err::TrdError> WriteBlankSD();

    static std::expected<void, Err::TrdError> IsSDPresent();
    
    //updates information written to file
    std::expected<void, Err::TrdError> UpdateSD(const TRD_SD_UPDATEFIELD& sd, bool autoWrite=true);
    
    std::expected<void, Err::TrdError> UpdateSDTblCount(u32 tblCount);
    std::expected<void, Err::TrdError> UpdateSDDynOffset(u64 dynOffset);
    std::expected<void, Err::TrdError> UpdateSDRegOffset(u64 tregOffset);
    
    
    
    static constexpr foffset_t GetSDAddress() noexcept;
    static constexpr foffset_t GetSDEnd() noexcept;
private:
    bool IChecksumValid(u32 crc, const TRD_SD& sd);
    std::expected<void, Err::TrdError> IWriteSD(bool blankWrite=false);
 //   std::expected<void, Err::TrdError> IRwAccessible();
    std::expected<void, Err::TrdError> IRwAccessible(Trd::Impl::TStreamInfo& fstr);
    std::expected<void, Err::TrdError> IValidateSDContent(const TRD_SD& sd);
    std::expected<void, Err::TrdError> IValidateTblAddr(const TRD_SD& sd); 
    
    
    //gets the starting position of SD table, private because by default SD table is written right after header
    //booyer moore horsepool
    //
    ////foffset_t FindSDAddress();
    
  



};


};