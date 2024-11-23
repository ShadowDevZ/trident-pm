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
    
    tsd.offsets = TRD_OffsetTblAlloc(tables, NULL);
    tsd.ids = TRD_IdTblAlloc(tables, NULL);
    if (tsd.offsets == NULL || tsd.ids == NULL) {
        printf("@!!!!MALLOC ERROR DEBUG\n");
        return 1;
    }
    
    trderr_t openRet = TRD_OpenPackage("package.tpx", &pkgi);
    if (openRet != TRDE_SUCCESS) {
        return 1;
    }


     TRD_WriteHeader(&pkgi, TRD_CT_LZ4, TRD_BF_AP_AMD64| TRD_BF_PLATF_LINUX\
    ,TRD_VersionToFormat(fver.Major,fver.Minor, \
    fver.Revision));
   
   

    trderr_t rcheck = TRD_ReadHeader(&pkgi);
    if (rcheck != TRDE_SUCCESS) {
        printf("@@Failed to read header\n");
    }
 
   
    
   
    printf("created package test\n");

    trderr_t hdrStatus = TRD_VerifyHeader(&pkgi);
    if (hdrStatus != TRDE_SUCCESS) {
        printf("checksum missmatch\n");
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

    

   
    TRD_InitDynamicTables(&pkgi, &tsd);
    
    uint64_t allocSize = sizeof(TRD_DYNTBL_TEST);
    TRD_DYNTBL_TEST* tst = malloc(allocSize);
    tst->test0 = 1337;
    tst->test1 = 69;
    if (tst == NULL){
        printf("!!!error\n");
    }
    TRD_DYNTBL_META meta_test = {
        .tuid0 = (tuid_t)0x1337,
        .revision = (tuid_t)0xfed5,
        .tuid1 = (tuid_t)0x1337,
        .dynTblLen = (uint64_t)allocSize
    };
   

    trd_err_t offsErr = TRD_GetSectionDescriptor(&pkgi, &tsd);
    printf("Reabback header status ::%d\n\n", offsErr);
    printf("Table count: %d\nTable flags 0x%X\n", tsd.offsets->tableCount, tsd.tableFlags);
    printf("table dump: \n");
    

    
    trderr_t gg =  TRD_AppendDynamicTable(&pkgi, &tsd, meta_test, tst);
    printf("gg=%d\n", gg);
    offsErr = TRD_GetSectionDescriptor(&pkgi, &tsd);
    for (int i=0;i < tsd.offsets->tableCount; ++i) {
        printf("\tID%lx::TC_%i%lx\n",tsd.ids->tableSeekIds[i],i, tsd.offsets->tableSeekOffset[i]);
    }

   
   
    TRD_FinitDynamicTables(&pkgi, &tsd);
    if (TRD_FinishFile(&pkgi) != TRDE_SUCCESS) {
        printf("!!!!corrupted file!!!\n");
    };

    free(tst);
    free(tsd.offsets);
    TRD_ClosePackage(&pkgi);
  
    
    return 0;
}