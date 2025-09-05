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



class TRDSecDesc {
private:
    std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fstrInfo;

public:
    LTSTATUS::TridentError e;
    TRDSecDesc(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) :
    fstrInfo(fStreamInfo) {}
    TRDSecDesc(const TRDSecDesc& other) : fstrInfo(other.fstrInfo) {}
    TRDSecDesc(TRDSecDesc&& other) : fstrInfo(std::move(other.fstrInfo)) {}
    //creates blank section
    bool InitializeSD();
    bool IsSDPresent();
    
    bool ReadSD(TRD_SD& sd);
    //updates information written to file
    bool UpdateSD(const TRD_SD& sd);

private:
bool IWriteRawSD(const TRD_SD& sd);
bool IReadRawSD(TRD_SD& sd);
//modifies written information, if present
bool IModifySD(rva_t rva);
//gets the starting position of SD table
bool GetSDAddress(rva_t& rvaOut);



};


};