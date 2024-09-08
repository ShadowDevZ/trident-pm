#include "sections.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
const uint64_t TRD_SECDESC_START_TOK = 0x6bf95b011917d968;
const uint64_t TRD_SECDESC_END_TOK =  0x9b7b1a36ca422afc;

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
    sd.tableFlags = 0xFFFFFFFF;

    // Allocate memory for tableSeekOffset
    sd.tableSeekOffset = (uint64_t *)malloc(sizeof(uint64_t) * tablesMax);
    if (sd.tableSeekOffset == NULL) {
        return TRDE_MALLOC_FAIL;
    }

    //example values for debugging 
   
   for (int i = 0; i < tablesMax; ++i) {
        sd.tableSeekOffset[i] = 0xDFFF83;
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