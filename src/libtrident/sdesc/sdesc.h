#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "ccattribs.h"
#include "fstreaminfo.h"
#include "pkgio.h"
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





class TRDSecDesc {
private:
    std::weak_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> wFstr;
    TRD_SD secDescInternal;

public:
    LTSTATUS::TridentError e;
    TRDSecDesc(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDSecDesc(const TRDSecDesc& other) : wFstr(other.wFstr) {}
    TRDSecDesc(TRDSecDesc&& other) : wFstr(std::move(other.wFstr)) {}
    
    const TRD_SD& GetInternal() const { 
        return secDescInternal;
    }
    TRD_SD& GetInternal() { 
        return secDescInternal;
    }
   
    
   //creates blank section
   //we cant really create it like header because there is 0 initial information to append during sequentional 
   //initialization as there are 0 tables
    
   //Create()
    bool WriteBlankSD();

    bool IsSDValid();
    static LTSTATUS::LTSTATUS IsSDPresent();
    
    bool ReadSD(TRD_SD& sd);
    //updates information written to file
    bool UpdateSD(const TRD_SD_UPDATEFIELD& sd);
    
    bool UpdateSDTblCount(u32 tblCount);
    bool UpdateSDDynOffset(u64 dynOffset);
    bool UpdateSDRegOffset(u64 tregOffset);
    
    
    bool CreateSDAtOffset(foffset_t offset);
    static foffset_t GetSDAddress();
    static foffset_t GetSDEnd();
private:
    u32 ICalculateChecksum();
    //gets the starting position of SD table, private because by default SD table is written right after header
    //booyer moore horsepool
    //
    foffset_t FindSDAddress();
    
  



};


};