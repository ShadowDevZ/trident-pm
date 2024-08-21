#include "libtrident.h"
#include "trheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buildflg.h"

uint32_t TRD_HeaderChecksum(const TRPD_HEADER *hdr) {
    uint32_t sum = 0;
    const uint8_t *bytes = (const uint8_t *)hdr;
    
    // Calculate checksum for the part before the checksum field
    for (size_t i = 0; i < sizeof(hdr->magic) + sizeof(hdr->fmtVersion) + sizeof(hdr->compressionType) + sizeof(hdr->buildFlags); i++) {
        sum += bytes[i];
    }
    
    // Calculate checksum for the part after the checksum field
    for (size_t i = sizeof(hdr->magic) + sizeof(hdr->fmtVersion) + sizeof(hdr->compressionType) + sizeof(hdr->buildFlags) + sizeof(hdr->headerChecksum) + sizeof(hdr->padding); 
         i < sizeof(TRPD_HEADER); 
         i++) {
        sum += bytes[i];
    }

    return sum;
}

uint16_t TRD_VersionToFormat(uint8_t major, uint8_t minor, uint8_t revision) {
    //00 00 0   mj=12 mi=5 rv=2
    //12052
    if (major > 99 || minor > 99 || revision > 9) {
        return 0;
    }

    char buff[6];
    snprintf(buff, sizeof(buff), "%02u%02u%01u", major,minor,revision);
    uint16_t formatted = (uint16_t)strtoul(buff,NULL ,10);
    if (formatted == UINT16_MAX || formatted == 0) {
        return 0;
    }
    return formatted;

}
_INTERNALF_ uint8_t StrCombineDigits(char c1, char c2) {
    char buff[3];
    snprintf(buff, sizeof(buff), "%c%c", c1,c2);
   
    uint8_t formatted = (uint8_t)strtoul(buff, NULL, 10);
    if (formatted == 0 || formatted > 99) {
        return 0;
    }
    return formatted;
}
TRPD_PKG_VERSION TRD_FormatToVersion(uint16_t fmt) {
    TRPD_PKG_VERSION ver;
    char buff[6];
    snprintf(buff, sizeof(buff), "%d", fmt);
    ver.Major = StrCombineDigits(buff[0], buff[1]);
    ver.Minor = StrCombineDigits(buff[2], buff[3]);
    ver.Revision = StrCombineDigits(buff[4],0);
  
  
    return ver;
}
//todo error check and return status
void TRD_WriteHeader(_TRD_PKGI* pkg, uint8_t comprType, uint16_t fmtVersion) {
    if (pkg == NULL){
        return;
    }
    TRPD_HEADER hdr = {0};
   
    memcpy(hdr.magic, TRD_PKG_MAGIC, strlen(TRD_PKG_MAGIC));
    hdr.fmtVersion = fmtVersion;
    hdr.compressionType = comprType;
   hdr.buildFlags = TRD_BF_PLATF_LINUX | TRD_BF_FMT_LE | \
    TRD_BF_DEBUG | TRD_BF_AP_AMD64;
    memset(hdr.padding, 0, sizeof(hdr.padding));
    hdr.headerChecksum = TRD_HeaderChecksum(&hdr);
    

    pkg->hdr = hdr;


    

}