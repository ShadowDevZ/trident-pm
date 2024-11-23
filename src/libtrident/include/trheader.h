#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "libtrident.h"
#include "trderr.h"
typedef uint64_t tuid_t;

#define TRD_PKG_MAGIC "\223TRD!\r\n"
#define TRD_HDR_PAD1_SIZE 6
#define TRD_HDR_PAD2_SIZE 4
#define TRD_FMT_VER 0x1C
#define TRD_EOFID_MAGIC 0xC92C155AB1E9FF1E

typedef struct {
    char magic[sizeof(TRD_PKG_MAGIC)];
    uint16_t fmtVersion;
    uint8_t compressionType;
    uint32_t buildFlags;
    uint32_t headerChecksum;
    uint8_t padding[TRD_HDR_PAD1_SIZE];
    uint64_t fileLen;
    uint8_t padding2[TRD_HDR_PAD2_SIZE];
    /*
    File named '.!${FILENAME}.tpx.lck' must exist containing the PID of the parrent accessing the file
    if the file doesnt exist or the PID points to other or non existing process the lock has to be removed
    this field is purely just a hint for the client software
    to not attempt to write any data and should always check
    the file lock before attempting to parse any data to avoid
    deadlocks
*/
    bool lock;

}__STRUCT_PACK TRPD_HEADER;

/*
FILE SPECIFICATION
@TRD SPEC
@VERSION 1.0


|----------------------------------------------+
|                    + HEADER +                |
+---------------+------------------+-----------+
|  MAGIC NUMBER |  FORMAT VERSION  |  COMPR    |
+---------------+------------------------------+
|  BUILD INFO FLAGS  |  CHKSUM  |   PADDING    | 
+--------------------+----------+--------------+
|  FILE_LENGTH       | PADDING  |  FILE_LOCK   | 
+---------------------+--------+---------------+
|        SECTION DESCRIPTOR START TOKEN        |
+-----------+--------------+-------------------+
|  TBL_CNT  |  TABLE FLAGS | TABLE PTR OFFSET  |
+-----------+--------------+-------------------+
|        SECTION DESCRIPTOR END TOKEN          |
+----------------------------------------------+
|           + DYNAMIC TBL SECTION +            |
+--------+------------+--------------+---------+
|  TUID  |  REVISION  |     [...]    |  TUID   |
+--------+------------+--------------+---------+
|          +  DYNAMIC TBL SECTION END +        |
+--------------------+-------------------------+
|    END OF FILE ID  |          PADDING        | 
+--------------------+-------------------------+



*/


typedef struct{
    uint8_t Major, Minor, Revision;
}__STRUCT_PACK TRPD_PKG_VERSION;


typedef struct {
    uint16_t tableCount;
    uint64_t tableSeekOffset[];
}__STRUCT_PACK _TRD_TABLE_OFFSETS;

typedef struct {
    char _dummy;
    tuid_t tableSeekIds[];
}__STRUCT_PACK _TRD_TABLE_IDS;

typedef struct {

    //which tables are present ? Binary flags
    uint32_t tableFlags; 
    //Seek offset to every table if present otherwise 0
    _TRD_TABLE_OFFSETS* offsets;
    _TRD_TABLE_IDS* ids;
    

}__STRUCT_PACK TRD_SECTION_DESCRIPTOR;



#include <stdio.h>

typedef struct {
    FILE *pkgHandle;
    TRPD_HEADER hdr;
    TRD_SECTION_DESCRIPTOR secDesc;
    //current id of table to append to
    uint32_t currentTable;
    uint64_t idtblOffset;
   //TRD_TABLES[TABLE_MAX];
   //TRD_MANIFEST_INFORMATION...
    

    uint64_t eofid;
    

}__STRUCT_PACK _TRD_PKGI;


trderr_t TRD_WriteHeader(_TRD_PKGI* pkg, uint8_t comprType, uint32_t buildFlags, uint16_t fmtVersion);
trderr_t TRD_ReadHeader(_TRD_PKGI* pkg);
uint32_t TRD_HeaderChecksum(const TRPD_HEADER *hdr);
uint16_t TRD_VersionToFormat(uint8_t major, uint8_t minor, uint8_t revision);
TRPD_PKG_VERSION TRD_FormatToVersion(uint16_t fmt);

bool TRD_CheckHeader(TRPD_HEADER* hdr);
TRPD_HEADER TRD_GetHeader();
trderr_t TRD_FinishFile(_TRD_PKGI* pkg);

trderr_t TRD_OpenPackage(const char* path, _TRD_PKGI* pkg);
trderr_t TRD_ClosePackage(_TRD_PKGI* pkg);
trderr_t TRD_VerifyHeader(_TRD_PKGI* pkg);
trderr_t TRD_AddFileLength(_TRD_PKGI* pkg, uint64_t len);
#include "trderr.h"
static inline size_t trd_fwrite(const void* ptr, size_t size, size_t nmemb, _TRD_PKGI* pkg) {
    size_t res = fwrite(ptr, size, nmemb, pkg->pkgHandle);
    trderr_t val = TRD_AddFileLength(pkg, size * nmemb);
    if (val != 0) {
        return 0;
    }
    return res;
}
