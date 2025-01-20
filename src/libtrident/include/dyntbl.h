#pragma once
#include <stdint.h>

#include "trheader.h"
#include "sections.h"
//todo add checks if section descriptor is valid
typedef void* trd_dyntbl_t;
typedef uint64_t tuid_t;
//table UID, unique for each different type


const uint64_t TRD_DYNSEC_START_TOK = 0x13d6f63458f6b9f4;
const uint64_t TRD_DYNSEC_END_TOK =   0x4dfbf705853ce8e2;

typedef struct {
    tuid_t tuid0;
    uint16_t revision;
    uint64_t dynTblLen;
    tuid_t tuid1;
}__STRUCT_PACK TRD_DYNTBL_META;


typedef struct {
    int test0;
    int test1;
}__STRUCT_PACK TRD_DYNTBL_TEST;
#define TUID_DYNTBL 0xabcd
typedef struct {
    size_t size;
    unsigned char* data;

}__STRUCT_PACK TRD_RAWBIN_TBL;


static inline bool TRD_DynTblPresent(_TRD_PKGI* pkg) {
    int64_t offset = 0;
    TRD_Val2Offset(pkg, TRD_DYNSEC_START_TOK, &offset);
    if (offset == -1)
        return false;
    return true;
}

trderr_t TRD_InitDynamicTables(_TRD_PKGI* pkg ,TRD_SECTION_DESCRIPTOR* secdesc);
trderr_t TRD_AppendDynamicTable(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* secDesc, TRD_DYNTBL_META meta, trd_dyntbl_t dtbl);
trderr_t TRD_GetDynamicTable(TRD_SECTION_DESCRIPTOR* secDesc, tuid_t tuid, trd_dyntbl_t* tblOut);
trderr_t TRD_FinitDynamicTables(_TRD_PKGI* pkg ,TRD_SECTION_DESCRIPTOR* secdesc);

trderr_t TRD_ReadDynamicTable(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* secDesc, TRD_DYNTBL_META* meta,tuid_t tuid, trd_dyntbl_t dtbl, int offset);