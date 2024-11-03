#include "dyntbl.h"
#include "trderr.h"


trderr_t __TRD_DynamicTables(_TRD_PKGI* pkg, bool init,TRD_SECTION_DESCRIPTOR* secdesc) {
    if (secdesc == NULL || pkg == NULL)
        return TRDE_NULL;
    if (pkg->pkgHandle == NULL)
        return TRDE_NOFILE;
    fseek(pkg->pkgHandle, 0, SEEK_END);

    int64_t offset = 0;
    if (init) {
        TRD_Val2Offset(pkg, TRD_DYNSEC_START_TOK, &offset);
    }
    else {
        TRD_Val2Offset(pkg, TRD_DYNSEC_END_TOK, &offset);
    }

    //dynamic table already exists
    if (offset != -1){    
        return TRDE_TOKEN_INV;
    }
    if (init) {
        if (!trd_fwrite(&TRD_DYNSEC_START_TOK, sizeof(TRD_DYNSEC_START_TOK), 1, pkg))
            return TRDE_SUCCESS;
        
    }
    else {
        if (!trd_fwrite(&TRD_DYNSEC_END_TOK, sizeof(TRD_DYNSEC_END_TOK), 1, pkg))
            return TRDE_SUCCESS;
    }

    
    return TRDE_IO_FAIL;
}
trderr_t TRD_InitDynamicTables(_TRD_PKGI* pkg ,TRD_SECTION_DESCRIPTOR* secdesc) {
    return __TRD_DynamicTables(pkg, true, secdesc);
}
trderr_t TRD_FinitDynamicTables(_TRD_PKGI* pkg ,TRD_SECTION_DESCRIPTOR* secdesc) {
    return __TRD_DynamicTables(pkg, false, secdesc);
}

trderr_t TRD_AppendDynamicTable(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* secDesc, TRD_DYNTBL_META meta, trd_dyntbl_t dtbl) {
    if (!TRD_DynTblPresent(pkg))
        return TRDE_NOSECTION;
    if (pkg == NULL || secDesc == NULL || dtbl == NULL)
        return TRDE_NULL;
    if (pkg->pkgHandle == NULL)
        return TRDE_NOFILE;
   //todo roll back to the start via fseek and check instead of blindly trusting it wasnt
   //manipulated, maybe adjust the function TRDval2offset to not rewind the file by default
   //roll back like few bytes and then check
    fseek(pkg->pkgHandle, 0, SEEK_END);
   



    if (trd_fwrite(&meta.tuid0, sizeof(meta.tuid0), 1, pkg) != 1)
        return TRDE_IO_FAIL;
    if (trd_fwrite(&meta.revision, sizeof(meta.revision), 1, pkg) != 1)
        return TRDE_IO_FAIL;

    //dynamic table start
    if (trd_fwrite(&dtbl, sizeof(meta.dynTblLen), 1, pkg) != 1)
        return TRDE_IO_FAIL;

    //dynamic table end
    if (trd_fwrite(&meta.dynTblLen, sizeof(meta.dynTblLen), 1, pkg) != 1)
        return TRDE_IO_FAIL;
    if (trd_fwrite(&meta.tuid1, sizeof(meta.tuid1), 1, pkg) != 1)
        return TRDE_IO_FAIL;
    
    return TRDE_SUCCESS;
    

}