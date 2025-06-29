#include <stdio.h>
#include <string.h>
#include "builder.h"
#include "libtrident.h"
#include "buildflg.h"
#include "sections.h"
#include "dyntbl.h"
#include "xmlparse.h"
#include "io.h"
int main(void) {
   

    printf("Trident Package Builder %s\n", TRD_BUILDER_VERSION);

    _TRD_PKGI pkgi = {0};
    TRPD_PKG_VERSION fver = {1, 0, 1};

    uint16_t tables = 10;
    TRD_SECTION_DESCRIPTOR tsd = {0};
    tsd.tableFlags = 0xDEADBEEF;

    tsd.offsets = TRD_OffsetTblAlloc(tables, NULL);
    tsd.ids = TRD_IdTblAlloc(tables, NULL);

    if (tsd.offsets == NULL || tsd.ids == NULL) {
        printf("@!!!! MALLOC ERROR DEBUG\n");
        return 1;
    }

    trderr_t openRet = TRD_OpenPackage("package.tpx", &pkgi);
    TSASSERT(openRet, "Failed to open package");

    TRD_WriteHeader(
        &pkgi,
        TRD_CT_LZ4,
        TRD_BF_AP_AMD64 | TRD_BF_PLATF_LINUX,
        TRD_VersionToFormat(fver.Major, fver.Minor, fver.Revision)
    );

    trderr_t rcheck = TRD_ReadHeader(&pkgi);
    TSASSERT(rcheck, "Failed to read header");

    printf("created package test\n");

    trderr_t hdrStatus = TRD_VerifyHeader(&pkgi);
    TSASSERT(hdrStatus, "Checksum mismatch");
    printf("checksum verified\n");

    TRD_SetLastError(TRDE_SUCCESS);
    trderr_t le = TRD_GetLastError();
    printf("Status: %u [%s]\n", le, le == TRDE_SUCCESS ? "OK" : "FAIL");

    
    for (int i = 0; i < tables; ++i) {
        tsd.offsets->tableSeekOffset[i] = 0x999;
    }

    trderr_t a = TRD_GenerateSectionHeader(&pkgi, &tsd, tables);
    printf("Generated header status :: %d\n\n", a);

    TRD_InitDynamicTables(&pkgi, &tsd);

    uint64_t allocSize = sizeof(TRD_DYNTBL_TEST);
    TRD_DYNTBL_TEST *tst = malloc(allocSize);
    TEASSERT(tst, NULL, "Dyntbl test failed to allocate memory");

    tst->test0 = 1337;
    tst->test1 = 69;

    TRD_DYNTBL_META meta_test = {
        .tuid0 = (tuid_t)0x1337,
        .crc32 = 0,
        .revision = (tuid_t)0xfed5,
        .tuid1 = (tuid_t)0x1337,
        .dynTblLen = (uint64_t)allocSize
    };

    trderr_t offsErr = TRD_GetSectionDescriptor(&pkgi, &tsd);
    printf("Readback header status :: %d\n\n", offsErr);
    printf("Table count: %d\nTable flags: 0x%X\n", tsd.offsets->tableCount, tsd.tableFlags);

    printf("table dump:\n");
    trderr_t dynTblRet = TRD_AppendDynamicTable(&pkgi, &tsd, meta_test, tst);
    printf("TSD before mod=0x%lx\n", tsd.offsets->tableSeekOffset[0]);
    printf("dynTblRet = %d\n", dynTblRet);
   

    offsErr = TRD_GetSectionDescriptor(&pkgi, &tsd);
    for (int i = 0; i < tsd.offsets->tableCount; ++i) {
        printf("\tID %lx :: SEEK %i_%lx\n", tsd.ids->tableSeekIds[i], i, tsd.offsets->tableSeekOffset[i]);
    }
    
    TRD_DYNTBL_TEST *readTable = malloc(meta_test.dynTblLen);
    TEASSERT(readTable, NULL, "ReadTable test failed to allocate memory");

    TRD_ReadDynamicTable(&pkgi, &tsd, &meta_test, 0x1337, readTable);

    _TrdPrintMeta(meta_test);
    printf("TestTableArgs=%d.%d\n", tst->test0, tst->test1);
    free(tst);
    free(readTable);

    // Read binary file
    char *file = "test.tar";
    FILE *fileToCompress = fopen(file, "rb");
    if (!fileToCompress) {
        perror("Failed to open file");
        return 1;
    }

    fseek(fileToCompress, 0, SEEK_END);
    size_t fileToCompressSize = ftell(fileToCompress);
    uint64_t allocSizeX = sizeof(TRD_RAWBIN_TBL);
    fseek(fileToCompress, 0, SEEK_SET);

    TRD_RAWBIN_TBL *raw = malloc(allocSizeX);
    raw->data = (unsigned char *)malloc(fileToCompressSize);
    raw->size = fileToCompressSize;

    if (raw == NULL || raw->data == NULL) {
        perror("malloc()");
        fclose(fileToCompress);
        return 1;
    }

    printf("%lu\n", raw->size);

    if (fread(raw->data, raw->size, 1, fileToCompress) != 1) {
        printf("read error\n");
        fclose(fileToCompress);
        free(raw->data);
        free(raw);
        return 1;
    }
    fclose(fileToCompress);

    // Just temporary mess for testing without any free()

    TRD_DYNTBL_META meta_fileToCompress = {
        .tuid0 = (tuid_t)0x1111,
        .crc32 = 0,
        .revision = (tuid_t)0xbeef,
        .tuid1 = (tuid_t)0x1111,
        .dynTblLen = (uint64_t)(allocSizeX)+(raw->size)
    };

    int appendDynamicRet = TRD_AppendDynamicTable(&pkgi, &tsd, meta_fileToCompress, raw);
  
    free(raw->data);
    free(raw);
    printf("appendDynamicRet:: %d\n", appendDynamicRet);

    offsErr = TRD_GetSectionDescriptor(&pkgi, &tsd);
    for (int i = 0; i < tsd.offsets->tableCount; ++i) {
        printf("\tID %lx :: SEEK %i_%lx\n", tsd.ids->tableSeekIds[i], i, tsd.offsets->tableSeekOffset[i]);
    }

    TRD_RAWBIN_TBL *readTableX = malloc(allocSizeX);
    memset(readTableX, 0, sizeof(TRD_RAWBIN_TBL));
    readTableX->data = (unsigned char *)malloc(fileToCompressSize);
    readTableX->size = 1722;

    printf("FileToCompressSize %lu\n", fileToCompressSize);

    if (readTableX == NULL || readTableX->data == NULL) {
        printf("@@malloc error\n");
        return TRDE_MALLOC_FAIL;
    }

    TRD_DYNTBL_META meta_read = {0};

    TRD_ReadDynamicTable(&pkgi, &tsd, &meta_read, 0x1111, readTableX);

    _TrdPrintMeta(meta_read);

    FILE *outtar = fopen("tarout.tar", "wb");
    if (fwrite(readTableX->data, readTableX->size, 1, outtar) != 1) {
        perror("error");
        return 1;
    }
    bool bfl = ValidateXML("/home/shadow/Projects/trident-pm/testing/xml/Manifest0.xml",
        "/home/shadow/Projects/trident-pm/testing/xml/Manifest.xsd");
    printf(" bFile=%d\n", bfl);
    if (!bfl) {
        return 1;
    }
     const char* filename = "/home/shadow/Projects/trident-pm/testing/xml/Manifest0.xml";
     size_t sz = 0;
     char* txt = ReadFileToBuffer(filename, &sz);
     if (txt == NULL) {
        return 1;
     }
     printf("%s\n", txt);
     
    xmlInitParser();
   

  //  xmlDoc* doc = xmlReadFile(filename, NULL, 0);
   xmlDoc* doc = xmlReadMemory(txt, sz, NULL, NULL, 0); 
   if (doc == NULL) {
        printf("Failed to parse %s\n", filename);
        return 1;
    }

    xmlNode* root = xmlDocGetRootElement(doc);
    if (!root) {
        printf("Empty XML document\n");
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return 1;
    }

    ProcessXMLNode(root->children, 0);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    free(txt);



    free(readTableX->data);
    free(readTableX);
    fclose(outtar);

    free(tsd.ids);
    free(tsd.offsets);

    TRD_FinitDynamicTables(&pkgi, &tsd);
    TSASSERT(TRD_FinishFile(&pkgi), "Corrupted file");

    _TrdPrintTable();
    TRD_ClosePackage(&pkgi);

    return 0;
}
