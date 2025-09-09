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


constexpr int TRD_SECTIONSD_SIZE = sizeof(TRD_SD);


class TRDSecDesc {
private:
    std::weak_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> weakFstr;

public:
    LTSTATUS::TridentError e;
    TRDSecDesc(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) :
    weakFstr(fStreamInfo) {}
    TRDSecDesc(const TRDSecDesc& other) : weakFstr(other.weakFstr) {}
    TRDSecDesc(TRDSecDesc&& other) : weakFstr(std::move(other.weakFstr)) {}
    //creates blank section
    bool BlankSD();
    bool IsSDPresent();
    
    bool ReadSD(TRD_SD& sd);
    //updates information written to file
    bool UpdateSD(const TRD_SD& sd);

private:
    bool IWriteRawSD(const TRD_SD& sd);
    bool IReadRawSD(TRD_SD& sd);
    //modifies written information, if present
    bool IModifySD(foffset_t off);
    //gets the starting position of SD table
    foffset_t IGetSDAddress();
  



};


};