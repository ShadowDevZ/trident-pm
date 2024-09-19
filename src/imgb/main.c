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
   // TRD_Wri
    trderr_t a = TRD_GenerateSectionHeader(&pkgi, 10);
    printf("%d\n",a);

    TRD_SECTION_DESCRIPTOR tsd = {0};
    trd_err_t offsErr = TRD_GetSectionDescriptor(&pkgi, &tsd);
    printf("secdesc: %d", offsErr);
    printf("\n\t::%u\n\t::%u\n", tsd.tableCount, tsd.tableFlags);

     fclose(f);
  
    
    return 0;
}