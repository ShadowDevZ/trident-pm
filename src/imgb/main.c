#include "stdio.h"
#include "builder.h"
#include "libtrident.h"
#include <string.h>
#include "buildflg.h"
#include "sections.h"
#include "dyntbl.h"
#include <time.h>
int main() {
    printf("Trident Package Builder %s\n", TRD_BUILDER_VERSION);
    _TRD_PKGI pkgi = {0};
    TRPD_PKG_VERSION fver = {1,0,1};
   

     uint16_t tables = 10;
     TRD_SECTION_DESCRIPTOR tsd = {0};
     tsd.tableFlags = 0xDEADBEEF;
    
    tsd.offsets = TRD_OffsetTblAlloc(tables);
    if (tsd.offsets == NULL) {
        printf("@!!!!MALLOC ERROR DEBUG\n");
        return 1;
    }
    
    FILE* f = fopen("package.tpx", "wb+");
    if (f == NULL) {
        perror("Failed to open file\n");
        return 1;
    }
    //todo add interface for IO operations
    pkgi.pkgHandle = f;

     TRD_WriteHeader(&pkgi, TRD_CT_LZ4, TRD_BF_AP_AMD64| TRD_BF_PLATF_LINUX\
    ,TRD_VersionToFormat(fver.Major,fver.Minor, \
    fver.Revision));
    
   
    printf("created package test\n");

    uint32_t hsum = TRD_HeaderChecksum(&pkgi.hdr);
    if (hsum != pkgi.hdr.headerChecksum) {
        printf("checksum missmatch\nog: %u cal: %u", pkgi.hdr.headerChecksum, hsum);
    }
    else{
        printf("checksum verified\n");
    }
    TRD_SetLastError(TRDE_SUCCESS);
    trderr_t le = TRD_GetLastError();
    printf("Status: %u[%s]\n", le, le == TRDE_SUCCESS ? "OK":"FAIL");
    srand(time(NULL));
    for (int i=0; i < tables; ++i) {
        tsd.offsets->tableSeekOffset[i] = rand() % (126226 + 1 - 5432) + 5432;
    }

    trderr_t a = TRD_GenerateSectionHeader(&pkgi,&tsd, tables);
    printf("Generated header status ::%d\n\n",a);

   

    trd_err_t offsErr = TRD_GetSectionDescriptor(&pkgi, &tsd);
    printf("Reabback header status ::%d\n\n", offsErr);
    printf("Table count: %d\nTable flags 0x%X\n", tsd.offsets->tableCount, tsd.tableFlags);
    printf("table dump: \n");
    for (int i=0;i < tsd.offsets->tableCount; ++i) {
        printf("\t::TC_%i%lx\n",i, tsd.offsets->tableSeekOffset[i]);
    }
    TRD_InitDynamicTables(&pkgi, &tsd);
    
    uint64_t allocSize = sizeof(TRD_DYNTBL_TEST);
    TRD_DYNTBL_TEST* tst = malloc(allocSize);
    if (tst == NULL){
        printf("!!!error\n");
    }
    TRD_DYNTBL_META meta_test = {
        .tuid0 = 0x1337,
        .revision = 0xfed5,
        .tuid1 = 0x1337,
        .dynTblLen = allocSize
    };


    trderr_t gg =  TRD_AppendDynamicTable(&pkgi, &tsd, meta_test, tst);
    printf("gg=%d\n", gg);
    TRD_FinitDynamicTables(&pkgi, &tsd);

    free(tst);
    free(tsd.offsets);
    fclose(f);
  
    
    return 0;
}