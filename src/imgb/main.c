#include "stdio.h"
#include "builder.h"
#include "libtrident.h"
#include <string.h>
#include "buildflg.h"
#include "sections.h"
int main() {
    printf("Trident Package Builder %s\n", TRD_BUILDER_VERSION);
    _TRD_PKGI pkgi = {0};
    TRPD_PKG_VERSION fver = {1,0,1};
   

     uint16_t tables = 10;
     TRD_SECTION_DESCRIPTOR tsd = {0};
    uint64_t tableSeekOffsetSize = sizeof(_TRD_TABLE_OFFSETS) + ((sizeof(uint64_t)) * tables);
    tsd.offsets = malloc(tableSeekOffsetSize);
    if (tsd.offsets == NULL) {
        printf("@!!!!MALLOC ERROR DEBUG\n");
        return 1;
    }
    
    FILE* f = fopen("package.test", "wb+");
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

   
    trderr_t a = TRD_GenerateSectionHeader(&pkgi, tables);
    printf("Generated header status ::%d\n\n",a);

   

    trd_err_t offsErr = TRD_GetSectionDescriptor(&pkgi, &tsd);
    printf("Reabback header status ::%d\n\n", offsErr);
    printf("\n\t::%u\n\t::0x%X\n", tsd.offsets->tableCount, tsd.tableFlags);
    printf("::::%lu\n",tsd.offsets->tableSeekOffset[0]);


    free(tsd.offsets);
    fclose(f);
  
    
    return 0;
}