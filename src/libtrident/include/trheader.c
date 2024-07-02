#include "libtrident.h"
#include "trheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
uint32_t TRD_HeaderChecksum(const TRPD_HEADER *hdr) {
    uint32_t sum = 0;
    const uint8_t *bytes = (const uint8_t *)hdr;
    
    // Calculate checksum for the part before the checksum field
    for (size_t i = 0; i < sizeof(hdr->magic) + sizeof(hdr->fmtVersion) + sizeof(hdr->compressionType) + sizeof(hdr->buildFlags); i++) {
        sum += bytes[i];
    }
    
    // Calculate checksum for the part after the checksum field
    for (size_t i = sizeof(hdr->magic) + sizeof(hdr->fmtVersion) + sizeof(hdr->compressionType) + sizeof(hdr->buildFlags) + sizeof(hdr->headerChecksum); 
         i < sizeof(TRPD_HEADER); 
         i++) {
        sum += bytes[i];
    }

    return sum;
}
void TRD_WriteHeader() {
    TRPD_HEADER hdr = {0};
   
    memcpy(hdr.magic, TRD_PKG_MAGIC, sizeof(TRD_PKG_MAGIC));
    hdr.fmtVersion = 0xBA;
    hdr.compressionType = 4;
    hdr.buildFlags = 0xBEAFDAD;
    memset(hdr.padding, 0, sizeof(hdr.padding));
    hdr.headerChecksum = TRD_HeaderChecksum(&hdr);
    

    FILE* f = fopen("package.test", "wb");
    if (f == NULL) {
        perror("Failed to open file\n");
        return;
    }
    fwrite(&hdr, sizeof(TRPD_HEADER), 1,  f);
    fclose(f);
    printf("created package test\n");


    uint32_t hsum = TRD_HeaderChecksum(&hdr);
    if (hsum != hdr.headerChecksum) {
        printf("checksum missmatch\nog: %u cal: %u", hdr.headerChecksum, hsum);
    }
    else{
        printf("checksum verified\n");
    }

}