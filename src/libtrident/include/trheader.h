#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "libtrident.h"
#define TRD_PKG_MAGIC "TRD!"
#define TRD_HDR_PAD_SIZE 5
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
+----------------------------------------------+




*/

typedef struct{
    uint8_t Major, Minor, Revision;
}__TRIDENT_PACKED TRPD_PKG_VERSION;

typedef struct {
    uint64_t tableCount;
    uint64_t tableFlags;
    uint64_t tableOffset;

}__TRIDENT_PACKED TRD_SECTION_DESCRIPTOR;

typedef struct {
    char magic[sizeof(TRD_PKG_MAGIC)];
    uint16_t fmtVersion;
    uint8_t compressionType;
    uint32_t buildFlags;
    uint16_t headerChecksum;
    uint8_t padding[TRD_HDR_PAD_SIZE];

}__TRIDENT_PACKED TRPD_HEADER;

void TRD_WriteHeader();