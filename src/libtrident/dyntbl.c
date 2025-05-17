#include "dyntbl.h"
#include "trderr.h"
#include "vec/vec.h"
#include "checksum.h"

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
    if (pkg == NULL || secDesc == NULL || dtbl == NULL \
    || secDesc->ids == NULL || secDesc->offsets == NULL)
        return TRDE_NULL;
    if (pkg->pkgHandle == NULL)
        return TRDE_NOFILE;
    
    if (meta.tuid0 != meta.tuid1)
        return TRDE_INV_CHKSUM;
    
    if (TRD_DtblOffsetLookup(meta.tuid0) != -1){
        return TRDE_ALREXISTS;
    }

    uint32_t currentTable = pkg->currentTable;
    secDesc->ids->tableSeekIds[currentTable] = meta.tuid0;
    printf("%lx\n", secDesc->ids->tableSeekIds[currentTable]);


    
    fseek(pkg->pkgHandle, pkg->idtblOffset, SEEK_SET);

   
    if ( (trd_fwrite(&secDesc->ids->tableSeekIds, sizeof(_TRD_TABLE_IDS) + ((sizeof(uint64_t)) * secDesc->offsets->tableCount), 1, pkg)) != 1) {
        return TRDE_IO_FAIL;
    }

   //todo roll back to the start via fseek and check instead of blindly trusting it wasnt
   //manipulated, maybe adjust the function TRDVal2offset to not rewind the file by default
   //roll back like few bytes and then check
    fseek(pkg->pkgHandle, 0, SEEK_END);
   

    long currentSeek = ftell(pkg->pkgHandle);
    secDesc->offsets->tableSeekOffset[currentTable] = currentSeek;
    TRD_RegenerateTableOffsets(pkg, secDesc);
    
    

    //todo find the correct table seek offse4t and write it
    if (trd_fwrite(&meta.tuid0, sizeof(meta.tuid0), 1, pkg) != 1)
        return TRDE_IO_FAIL;

    if (trd_fwrite(&meta.revision, sizeof(meta.revision), 1, pkg) != 1)
        return TRDE_IO_FAIL;
    if (trd_fwrite(&meta.dynTblLen, sizeof(meta.dynTblLen), 1, pkg) != 1)
        return TRDE_IO_FAIL;
    
    
    CRC32 crc = UpdateTableChecksum(0, &meta, NULL, 0);
    
    
    switch (meta.tuid0)
    {
    case 0x1111:
    //todo call specific function to write the file instead
        TRD_RAWBIN_TBL* rw = (TRD_RAWBIN_TBL*)dtbl;
        printf("RAW_TBL_DATA_SIZE=%lu, RAW_TBL_DATA_LEN=%lu\n", rw->size, meta.dynTblLen);

       // TASSERT(rw->size+sizeof(TRD_RAWBIN_TBL), meta.dynTblLen, "Metadata size does not match the binary size");
        crc = UpdateTableChecksum(crc, NULL, rw->data, rw->size);

        if (trd_fwrite(&rw->size, sizeof(rw->size), 1, pkg) != 1) {
            return TRDE_IO_FAIL;
        }
        if (trd_fwrite(rw->data, rw->size, 1, pkg) != 1) {
            return TRDE_IO_FAIL;
        }
        break;
    
    default:
        if (trd_fwrite(&dtbl, meta.dynTblLen, 1, pkg) != 1)
            return TRDE_IO_FAIL;
        crc = UpdateTableChecksum(crc, NULL, dtbl, meta.dynTblLen);
        break;
    }
    meta.crc32 = crc;
    

    if (trd_fwrite(&meta.crc32, sizeof(meta.crc32), 1, pkg) != 1)
        return TRDE_IO_FAIL;
    if (meta.tuid0 == 0x1111) {
        printf("ORIGNAAAAAAAAAAAAAA: %x\n", meta.crc32);
    }
    //dynamic table end
    
    if (trd_fwrite(&meta.tuid1, sizeof(meta.tuid1), 1, pkg) != 1)
        return TRDE_IO_FAIL;
    
    ++currentTable;
    pkg->currentTable = currentTable;
    _TRD_DtblAddEntry(meta.tuid0);

    return TRDE_SUCCESS;
    

}


trderr_t TRD_ReadDynamicTable(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* secDesc, TRD_DYNTBL_META* meta,tuid_t tuid,
trd_dyntbl_t dtbl) {
    if (!TRD_DynTblPresent(pkg))
        return TRDE_NOSECTION;
    if (pkg == NULL || secDesc == NULL
    || secDesc->ids == NULL || secDesc->offsets == NULL  || dtbl == NULL)
        return TRDE_NULL;
    if (pkg->pkgHandle == NULL)
        return TRDE_NOFILE;
    int offset = TRD_DtblOffsetLookup(tuid);
    printf("current table offset index is %d\n", offset);
    if (offset == -1) {
        return TRDE_BAD_ARG;
    }
    
    long currentSeek = ftell(pkg->pkgHandle);

    uint16_t tblCount =  secDesc->offsets->tableCount;
    int tblIndex;
    for (tblIndex = 0; tblIndex < tblCount; ++tblIndex) {
       tuid_t id =  secDesc->ids->tableSeekIds[tblIndex];
       if (id == tuid) {

        printf("found table with tuid 0x%lx, i=%d\n", id,tblIndex);
        break;
       }
    }
    
    uint64_t tblSeek = secDesc->offsets->tableSeekOffset[offset];
  
    fseek(pkg->pkgHandle, tblSeek, SEEK_SET);
    

    
   
    TRD_DYNTBL_META metaRead = {0};
    if (fread(&metaRead.tuid0, sizeof(metaRead.tuid0), 1, pkg->pkgHandle) != 1) {
       goto fix_seek;
    }
   
    if (fread(&metaRead.revision, sizeof(metaRead.revision), 1, pkg->pkgHandle) != 1) {
       goto fix_seek;
    }
    if (fread(&metaRead.dynTblLen, sizeof(metaRead.dynTblLen), 1, pkg->pkgHandle) != 1) {
       goto fix_seek;
    }
    //dyntbl start
    CRC32 crc = UpdateTableChecksum(0, &metaRead, NULL, 0);
    switch (metaRead.tuid0)
    {
    case 0x1111:
    //todo call specific function to write the file instead
        TRD_RAWBIN_TBL* rw = (TRD_RAWBIN_TBL*)dtbl;
        if (rw == NULL) {
            printf("null ptr\n");
            break;
        }
        
      
       
        if (fread(&rw->size, sizeof(rw->size), 1, pkg->pkgHandle) != 1) {
             
            goto fix_seek;
        }
        
   
       if (fread(rw->data, rw->size, 1, pkg->pkgHandle) != 1) {
          
           goto fix_seek;
        }
        

       printf("RAW_TAR_SIZE=%lu\n", rw->size);
       
       printf("RAW_TAR_DATA=%s\n", rw->data);
       crc = UpdateTableChecksum(crc, NULL, rw->data, rw->size);
       
        
        break;
         
    
    default:
        if (fread(&dtbl, metaRead.dynTblLen, 1, pkg->pkgHandle) != 1) {
            goto fix_seek;       
        }
         crc = UpdateTableChecksum(crc, NULL, dtbl, metaRead.dynTblLen);
        break;
    }
    
    

    if (fread(&metaRead.crc32, sizeof(metaRead.crc32), 1, pkg->pkgHandle) != 1) {
        goto fix_seek;
    }
    
   
        printf("   crcOrignal: %x", crc);
        printf("   crcRead: %x", metaRead.crc32);
        TASSERT(crc, metaRead.crc32, "CRC32 missmatch");
    
     
    

    //dyntbl end

    if (fread(&metaRead.tuid1, sizeof(metaRead.tuid1), 1, pkg->pkgHandle) != 1) {
       goto fix_seek;
    }


    
   



    *meta = metaRead;
    fseek(pkg->pkgHandle, currentSeek, SEEK_SET);
    
    return TRDE_SUCCESS;
fix_seek:
    fseek(pkg->pkgHandle, currentSeek, SEEK_SET);
    return TRDE_IO_FAIL;
}
static vec_int_t g_dtblLookup;

trderr_t _TRD_DtblOffsetTableInit() {
    vec_init(&g_dtblLookup);
    return TRDE_SUCCESS;
}
trderr_t _TRD_DtblOffsetTableFree() {
    vec_deinit(&g_dtblLookup);
    return TRDE_SUCCESS;
}
trderr_t _TRD_DtblAddEntry(tuid_t tuid) {
    int i;
    vec_find(&g_dtblLookup, tuid, i);
    if (i != -1)
        return TRDE_ALREXISTS;


    vec_push(&g_dtblLookup, tuid);
    return TRDE_SUCCESS;
}
trderr_t _TRD_DtblDelEntry(tuid_t tuid) {
    int i;
    vec_find(&g_dtblLookup, tuid, i);
    if (i == -1)
        return TRDE_NULL;


    vec_remove(&g_dtblLookup, tuid);
    return TRDE_SUCCESS;
}

int TRD_DtblOffsetLookup(tuid_t tuid) {
    int i;
    vec_find(&g_dtblLookup, tuid, i);
    return i;
    
}
void _TrdPrintTable() {
    int i; tuid_t val;
    vec_foreach(&g_dtblLookup, val, i) {
    printf("LOOKUP_TUID ID:%x : TUID:%lx\n", i, val);
}   
}
void _TrdPrintMeta(TRD_DYNTBL_META meta) {
    printf("\n===META_DUMP===\n");
    printf(" ::tuid0=0x%lx\n", meta.tuid0);
    printf(" ::crc32=0x%08x\n", meta.crc32);
    printf(" ::revision=%d\n", meta.revision);
    printf(" ::length=%lu\n", meta.dynTblLen);
    printf(" ::tuid1=0x%lx", meta.tuid1);
    printf("\n===META_DUMP===\n");
}