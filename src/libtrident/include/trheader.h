#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "libtrident.h"
#define TRD_PKG_MAGIC "TRD!"
#define TRD_HDR_PAD_SIZE 5
#define TRD_FMT_VER 0x1C
#define TRD_EOFID 0xCCD55AB1E8FF1E
/*
FILE SPECIFICATION
@TRD SPEC
@VERSION 1.0


|----------------------------------------------+
|                    + HEADER +                |
+---------------+------------------+-----------+
|  MAGIC NUMBER |  FORMAT VERSION  |  COMPR    |
+---------------+------------------+-----------+
|  BUILD INFO FLAGS   | CHKSUM |   PADDING     | 
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
/*
typedef struct {
    uint32_t tuidStart;
    uint16_t revision;
    ///CUSTOM CONTENT HERE
    uint32_t tuidEnd;

}TRD_DYN_TBL;
*/

typedef struct{
    uint8_t Major, Minor, Revision;
}__STRUCT_PACK TRPD_PKG_VERSION;

typedef struct {
    uint16_t tableCount;
    uint32_t tableFlags; 
    uint64_t tableOffset;

}__STRUCT_PACK TRD_SECTION_DESCRIPTOR;

typedef struct {
    char magic[sizeof(TRD_PKG_MAGIC)];
    uint16_t fmtVersion;
    uint8_t compressionType;
    uint32_t buildFlags;
    uint32_t headerChecksum;
    uint8_t padding[TRD_HDR_PAD_SIZE];

}__STRUCT_PACK TRPD_HEADER;

typedef struct {
    TRPD_HEADER hdr;
    uint32_t secDescTokStart;
    TRD_SECTION_DESCRIPTOR secDesc;
    uint32_t secDescTokEnd;
    uint32_t dynSecTokStart;


    uint32_t dynSecTokEnd;

    uint64_t eofid;
    uint8_t padding[6];

}__STRUCT_PACK _TRD_PKGI;
uint32_t TRD_HeaderChecksum(const TRPD_HEADER *hdr);
uint16_t TRD_VersionToFormat(uint8_t major, uint8_t minor, uint8_t revision);
TRPD_PKG_VERSION TRD_FormatToVersion(uint16_t fmt);
void TRD_WriteHeader(_TRD_PKGI* pkg, uint8_t comprType, uint16_t fmtVersion);