#include "stdio.h"
#include "builder.h"
#include "libtrident.h"
#include <string.h>
int main() {
    printf("Trident Package Builder %s\n", TRD_BUILDER_VERSION);
    _TRD_PKGI pkgi = {0};
    TRPD_PKG_VERSION fver = {1,0,1};
    TRD_WriteHeader(&pkgi, TRD_CT_LZ4, TRD_VersionToFormat(fver.Major,fver.Minor, \
    fver.Revision));
    
    FILE* f = fopen("package.test", "wb");
    if (f == NULL) {
        perror("Failed to open file\n");
        return 1;
    }
    fwrite(&pkgi.hdr, sizeof(TRPD_HEADER), 1,  f);
    fclose(f);
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


  
    
    return 0;
}