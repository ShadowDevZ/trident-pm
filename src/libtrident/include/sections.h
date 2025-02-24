#pragma once
#include "trheader.h"
#include "trderr.h"
#include <stdlib.h>
#include <string.h>
trderr_t TRD_GenerateSectionHeader(_TRD_PKGI* pkg,TRD_SECTION_DESCRIPTOR* desc, uint16_t tablesMax);
trderr_t TRD_GetSectionDescriptor(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* tsd);
//finds corresponding SEEK offset in file if value is found
/*
_TRD_PKGI* pkg,    ->  pointer to the package
uint64_t value,    -> value which we are looking for
int64_t* offsetOut -> seek offset of the first found variable, if none is found -1 is returned

*/
trd_err_t TRD_Val2Offset(_TRD_PKGI* pkg, uint64_t value, int64_t* offsetOut);

static inline _TRD_TABLE_OFFSETS* TRD_OffsetTblAlloc(uint16_t tableCount, uint64_t* sizeOut) {
    uint64_t tableSeekOffsetSize = sizeof(_TRD_TABLE_OFFSETS) + (sizeof(uint64_t) * tableCount);
    TRD_SECTION_DESCRIPTOR secDesc;
     //the +2 is from valgrind which fixes memory leak, i have no idea why
    secDesc.offsets = (_TRD_TABLE_OFFSETS*)calloc(1,tableSeekOffsetSize + 2);
    

    if (sizeOut != NULL) {
        *sizeOut = tableSeekOffsetSize;
    }
    
    return secDesc.offsets;
}
static inline _TRD_TABLE_IDS* TRD_IdTblAlloc(uint16_t tableCount, uint64_t* sizeOut) {
    uint64_t tableIdSize= sizeof(_TRD_TABLE_IDS) + ((sizeof(uint64_t)) * tableCount);
    TRD_SECTION_DESCRIPTOR secDesc;
    //the +2 is from valgrind which fixes memory leak, i have no idea why
    secDesc.ids = (_TRD_TABLE_IDS*)calloc(1,tableIdSize + 2);
   
    if (sizeOut != NULL) {
        *sizeOut = tableIdSize;
    }
    return secDesc.ids;
}
trderr_t TRD_RegenerateTableOffsets(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* desc);