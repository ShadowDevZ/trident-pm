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
    *offsetOut = -1;
    return TRDE_FAILURE;

}



//Warning function provides allocated memory to tsd. This memory has to be freed
//by developer manually (tsd->offsets)
trderr_t TRD_GetSectionDescriptor(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* tsd) {
    if (pkg == NULL || tsd == NULL) {
        return TRDE_NULL;
    }
    if (pkg->pkgHandle == NULL) {
        return TRDE_NOFILE;
    }
    int64_t seekPos = ftell(pkg->pkgHandle);
    if (seekPos == -1) {
        return TRDE_IO_FAIL;
    }
    
    int64_t token = 0;
    
    
  
    trderr_t status =  TRD_Val2Offset(pkg, TRD_SECDESC_START_TOK, &token);
    rewind(pkg->pkgHandle);
    
    if (fseek(pkg->pkgHandle, token, SEEK_SET)) {
        
        return TRDE_IO_FAIL;
    }
    uint16_t tableCount = 0;

    //we cannot just read the whole struct because we are expecting to fill the
    //offset with the SEEK position
    
    if (fread(&tableCount, sizeof(tableCount), 1, pkg->pkgHandle) != 1) {
         fseek(pkg->pkgHandle, seekPos, SEEK_SET);
         return TRDE_IO_FAIL;
    }
    

    TRD_SECTION_DESCRIPTOR secDesc = {0};
    uint64_t tableSeekOffsetSize, tableIdSize = 0;
    secDesc.offsets = TRD_OffsetTblAlloc(tableCount, &tableSeekOffsetSize);

    secDesc.ids = TRD_IdTblAlloc(tableCount, &tableIdSize);
    
    if (secDesc.offsets == NULL || secDesc.ids == NULL) {
        return TRDE_MALLOC_FAIL;
    }

    secDesc.offsets->tableCount = tableCount;
  
   
    
    if (fread(&secDesc.tableFlags, sizeof(secDesc.tableFlags), 1, pkg->pkgHandle) != 1) {
         fseek(pkg->pkgHandle, seekPos, SEEK_SET);
         goto mclean;
         return TRDE_IO_FAIL;
    }

    
    size_t g = 0;
    if ((g = fread(&secDesc.offsets->tableSeekOffset, tableSeekOffsetSize, 1, pkg->pkgHandle)) != 1) {
        printf("got :: %lu, needed %d", g, secDesc.offsets->tableCount);
        goto mclean;
        return TRDE_IO_FAIL;
    }
    char _dummy = '\0';
    if (fread(&_dummy, 1, 1, pkg->pkgHandle) != 1) {
        printf("@@control byte\n");
        return TRDE_IO_FAIL;
    }
    if ((g = fread(&secDesc.ids->tableSeekIds, tableIdSize, 1, pkg->pkgHandle)) != 1) {
        printf("got :: %lu, needed %d", g, secDesc.offsets->tableCount);
        goto mclean;
        return TRDE_IO_FAIL;
    }

    for (int i = 0; i < tableCount; ++i) {
        printf("Readback [%d]: %lu\n", i, secDesc.offsets->tableSeekOffset[i]);
    }

 

    uint64_t endval = 0;
    if (fread(&endval, sizeof(endval), 1, pkg->pkgHandle) != 1) {
         fseek(pkg->pkgHandle, seekPos, SEEK_SET);
         goto mclean;
         return TRDE_IO_FAIL;
    }
    


    fseek(pkg->pkgHandle, seekPos, SEEK_SET);

    if (endval != TRD_SECDESC_END_TOK) {
        goto mclean;
        return TRDE_FILE_CORRUPTED;
    }
    
    *tsd = secDesc;

    //the developer has to free the memory manually after use
    //free(secDesc.offsets);
    return TRDE_SUCCESS;
mclean:
    free(secDesc.offsets);
    free(secDesc.ids);
    return TRDE_IO_FAIL;


}


//TRD_SECTION_DESCRIPTOR
trderr_t TRD_GenerateSectionHeader(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* desc, uint16_t tablesMax) {
    if (pkg == NULL)
        return TRDE_NULL;
    if (pkg->pkgHandle == NULL)
        return TRDE_NOFILE;
    if (desc == NULL) 
        return TRDE_BAD_ARG;

    // Write the start token
    if (trd_fwrite(&TRD_SECDESC_START_TOK, sizeof(TRD_SECDESC_START_TOK), 1, pkg) != 1) {
        
        return TRDE_IO_FAIL;
    }

    // Initialize section descriptor
    TRD_SECTION_DESCRIPTOR sd = {0};
   // sd.offsets.tableCount = tablesMax;
    sd.tableFlags = desc->tableFlags;


    // Allocate memory for tableSeekOffset
    
    
    sd.offsets = TRD_OffsetTblAlloc(tablesMax, NULL);
    if (sd.offsets == NULL) {
        return TRDE_MALLOC_FAIL;
    }
  
    
    sd.offsets->tableCount = tablesMax;

    //
   sd.ids = TRD_IdTblAlloc(tablesMax, NULL);
   
   if (sd.offsets == NULL) {
        return TRDE_MALLOC_FAIL;
   }

   for (int i = 0; i < tablesMax; ++i) {
        sd.offsets->tableSeekOffset[i] = desc->offsets->tableSeekOffset[i];
        sd.ids->tableSeekIds[i] = 0;
    }
    

   
   
    

    
    
  
    for (int i = 0; i < tablesMax; ++i) {
        printf("Writeback [%d]: %lu\n", i, sd.offsets->tableSeekOffset[i]);
    }
 


    // Write the structure's fixed fields first 
    uint16_t tblCount = sd.offsets->tableCount;
    
    if (trd_fwrite(&tblCount, sizeof(uint16_t), 1, pkg) != 1 ||
        trd_fwrite(&sd.tableFlags, sizeof(sd.tableFlags), 1, pkg) != 1) {
        goto free_reg;
        return TRDE_IO_FAIL;
    }
   
   
    size_t g = 0;
        
    // Write the dynamically allocated tableSeekOffset array
    if ( (g = trd_fwrite(&sd.offsets->tableSeekOffset, sizeof(_TRD_TABLE_OFFSETS) + ((sizeof(uint64_t)) * tablesMax), 1, pkg)) != 1) {
        printf("offset tables written==%lu, needed: %d\n", g, tablesMax);
        goto free_reg;
        return TRDE_IO_FAIL;
    }
    char _dummy = '\0';
    if (trd_fwrite(&_dummy, 1, 1, pkg) != 1) {
        printf("@@control byte\n");
        goto free_reg;
        return TRDE_IO_FAIL;
    }

    pkg->idtblOffset = ftell(pkg->pkgHandle);
  
    if ( (g = trd_fwrite(&sd.ids->tableSeekIds, sizeof(_TRD_TABLE_IDS) + ((sizeof(uint64_t)) * tablesMax), 1, pkg)) != 1) {
        printf("id tables written==%lu, needed: %d\n", g, tablesMax);
        goto free_reg;
        return TRDE_IO_FAIL;
    }
     

    // Write the end token
    if (trd_fwrite(&TRD_SECDESC_END_TOK, sizeof(TRD_SECDESC_END_TOK), 1, pkg) != 1) {
        goto free_reg;
        return TRDE_IO_FAIL;
    }

    
  
    free(sd.offsets);
    free(sd.ids);
    return TRDE_SUCCESS;

free_reg:
    free(sd.offsets);
    free(sd.ids);
    return TRDE_IO_FAIL;
}