#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "ccattribs.h"
#include "tstreaminfo.h"
#include "pkgio.h"
#include "sectioncommon.h"
namespace LibTrident::SectionDescriptor {

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





class TRDSecDesc : public LibTrident::Sections::SectionCommon<TRD_SD> {
private:
    std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr;
    TRD_SD secDescInternal;

public:
    LibTrident::Err::TridentError e;
    TRDSecDesc(std::shared_ptr<LibTrident::Tstream::TStreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDSecDesc(const TRDSecDesc& other) : wFstr(other.wFstr) {}
    TRDSecDesc(TRDSecDesc&& other) : wFstr(std::move(other.wFstr)) {}
    
    const TRD_SD& GetObject() const override { 
        return secDescInternal;
    }
    TRD_SD& GetObject() override { 
        return secDescInternal;
    }
    inline bool StatusOk()  override {
        return e.IsOk();
    }
    inline bool Write() override {
        return IWriteSD(false);
    }
   
    bool IsValid() override;
    bool Read() override;
    std::optional<TRD_SD> ReadBack() override;
   //creates blank section
   //we cant really create it like header because there is 0 initial information to append during sequentional 
   //initialization as there are 0 tables
    
   //Create()
    bool WriteBlankSD();

    static LibTrident::Err::Code IsSDPresent();
    
    //updates information written to file
    bool UpdateSD(const TRD_SD_UPDATEFIELD& sd, bool autoWrite=true);
    
    bool UpdateSDTblCount(u32 tblCount);
    bool UpdateSDDynOffset(u64 dynOffset);
    bool UpdateSDRegOffset(u64 tregOffset);
    
    
    
    static foffset_t GetSDAddress();
    static foffset_t GetSDEnd();
private:
    bool IChecksumValid(u32 crc, const TRD_SD& sd);
    bool IWriteSD(bool blankWrite=false);
    bool IRwAccessible();
    bool IRwAccessible(std::weak_ptr<LibTrident::Tstream::TStreamInfo> fstr);
    bool IValidateSDContent(const TRD_SD& sd);
    bool IValidateTblAddr(const TRD_SD& sd); 
    //gets the starting position of SD table, private because by default SD table is written right after header
    //booyer moore horsepool
    //
    foffset_t FindSDAddress();
    
  



};


};