#include "sections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <errno.h>
const uint64_t TRD_SECDESC_START_TOK = 0x6bf95b011917d968;
const uint64_t TRD_SECDESC_END_TOK =  0x9b7b1a36ca422afc;

trd_err_t TRD_Val2Offset(_TRD_PKGI* pkg, uint64_t value, int64_t* offsetOut) {
    if (pkg == NULL) {
        return TRDE_NULL;
    }
    if (pkg->pkgHandle == NULL) {
        return TRDE_NOFILE;
    }
    if (offsetOut == NULL) {
        return TRDE_NULL;
    }
    int64_t origSeek = ftell(pkg->pkgHandle);
    uint64_t buffer = 0;
    size_t readSize = 0;
   

    rewind(pkg->pkgHandle);
    while ((readSize = fread(&buffer, 1, sizeof(buffer), pkg->pkgHandle)) == sizeof(buffer)) {
        if (buffer == value) {
            *offsetOut = ftell(pkg->pkgHandle);
            fseek(pkg->pkgHandle, origSeek, SEEK_SET);
            return TRDE_SUCCESS;
        }
        
        fseek(pkg->pkgHandle, -7, SEEK_CUR);

    }
    fseek(pkg->pkgHandle, origSeek, SEEK_SET);

    return TRDE_FAILURE;

}



trderr_t TRD_GetSectionDescriptor(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* tsd) {
    if (pkg == NULL) {
        return TRDE_NULL;
    }
    if (pkg->pkgHandle == NULL) {
        return TRDE_NOFILE;
    }
    if (tsd == NULL) {
        return TRDE_NULL;
    }
    int64_t seekPos = ftell(pkg->pkgHandle);
    if (seekPos == -1) {
        return TRDE_IO_FAIL;
    }
    
    int64_t token = 0;
    TRD_SECTION_DESCRIPTOR secDesc = {0};
   
  
    trderr_t status =  TRD_Val2Offset(pkg, TRD_SECDESC_START_TOK, &token);
    rewind(pkg->pkgHandle);
    
    if (fseek(pkg->pkgHandle, token, SEEK_SET)) {
        
        return TRDE_IO_FAIL;
    }
    

    //we cannot just read the whole struct because we are expecting to fill the
    //offset with the SEEK position
    if (fread(&secDesc.tableCount, sizeof(secDesc.tableCount), 1, pkg->pkgHandle) != 1) {
         fseek(pkg->pkgHandle, seekPos, SEEK_SET);
         return TRDE_IO_FAIL;
    }
    if (fread(&secDesc.tableFlags, sizeof(secDesc.tableFlags), 1, pkg->pkgHandle) != 1) {
         fseek(pkg->pkgHandle, seekPos, SEEK_SET);
         return TRDE_IO_FAIL;
    }
   // uint64_t* offsets = malloc(secDesc.tableFlags * 8);

    //if (fread(offsets, sizeof(uint64_t), secDesc.tableCount, pkg->pkgHandle) != 8) {
      //   fseek(pkg->pkgHandle, seekPos, SEEK_SET);
        // return TRDE_IO_FAIL;
   // }
   

   // printf("%luu", secDesc.tableSeekOffset[0]);

   //just for now
    fseek(pkg->pkgHandle, 10*8, SEEK_CUR);

    uint64_t endval = 0;
    if (fread(&endval, sizeof(endval), 1, pkg->pkgHandle) != 1) {
         fseek(pkg->pkgHandle, seekPos, SEEK_SET);
         return TRDE_IO_FAIL;
    }


    fseek(pkg->pkgHandle, seekPos, SEEK_SET);

    if (endval != TRD_SECDESC_END_TOK) {
        return TRDE_FILE_CORRUPTED;
    }
    *tsd = secDesc;
    
    return TRDE_SUCCESS;



}


//TRD_SECTION_DESCRIPTOR
trderr_t TRD_GenerateSectionHeader(_TRD_PKGI* pkg, uint16_t tablesMax) {
    if (pkg == NULL) {
        return TRDE_NULL;
    }
    if (pkg->pkgHandle == NULL) {
        return TRDE_NOFILE;
    }

    // Write the start token
    if (fwrite(&TRD_SECDESC_START_TOK, sizeof(TRD_SECDESC_START_TOK), 1, pkg->pkgHandle) != 1) {
        
        return TRDE_IO_FAIL;
    }

    // Initialize section descriptor
    TRD_SECTION_DESCRIPTOR sd = {0};
    sd.tableCount = tablesMax;
    sd.tableFlags = 0xDEADED;

    // Allocate memory for tableSeekOffset
    sd.tableSeekOffset = (uint64_t *)malloc(sizeof(uint64_t) * tablesMax);
    if (sd.tableSeekOffset == NULL) {
        return TRDE_MALLOC_FAIL;
    }

    //example values for debugging 
   
   for (int i = 0; i < tablesMax; ++i) {
        sd.tableSeekOffset[i] = UINT64_MAX;
   }

    // Write the structure's fixed fields first 
    if (fwrite(&sd.tableCount, sizeof(sd.tableCount), 1, pkg->pkgHandle) != 1 ||
        fwrite(&sd.tableFlags, sizeof(sd.tableFlags), 1, pkg->pkgHandle) != 1) {
        free(sd.tableSeekOffset);
        return TRDE_IO_FAIL;
    }

    // Write the dynamically allocated tableSeekOffset array
    if (fwrite(sd.tableSeekOffset, sizeof(uint64_t), sd.tableCount, pkg->pkgHandle) != sd.tableCount) {
        free(sd.tableSeekOffset);
        return TRDE_IO_FAIL;
    }

    // Write the end token
    if (fwrite(&TRD_SECDESC_END_TOK, sizeof(TRD_SECDESC_END_TOK), 1, pkg->pkgHandle) != 1) {
        free(sd.tableSeekOffset);
        return TRDE_IO_FAIL;
    }

    
    free(sd.tableSeekOffset);
    
    return TRDE_SUCCESS;
}