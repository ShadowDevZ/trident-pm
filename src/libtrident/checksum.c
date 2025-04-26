#include "checksum.h"
#include <zlib.h>
//1st pass NULL, meta, data1, size1;; -> crc32
//nth pass, crc32, NULL, dataN, sizeN -> crc32
CRC32 UpdateTableChecksum(CRC32 rcrc, TRD_DYNTBL_META* meta, void* data, uint32_t size) {
    
    CRC32 crc = rcrc;
   
    
    if (meta != NULL) {
        crc = crc32(crc, (const unsigned char*)&meta->dynTblLen, sizeof(meta->dynTblLen));
       
    }
    if (data) {
        crc = crc32(crc, (const unsigned char*)data, size);
    }
    


   
    return crc;
}