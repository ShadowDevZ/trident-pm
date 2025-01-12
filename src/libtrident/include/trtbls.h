#pragma once
#include "libtrident.h"

typedef struct {
    uint16_t checksum;
    uint64_t tblFlags;
    uint64_t sectionSize;
    uint32_t uti;
    uint64_t tblOffset;
    uint64_t tblSize;
    uint16_t tblRevision;

}__STRUCT_PACK TRPD_TABLE_TEMPLATE;

enum TRPD_TABLE_LIST {
    TRPD_TBL_MANIFEST = 1 << 1,
    TRPD_TBL_RAW_BIN = 1 << 2,
    TRPD_TBL_EXT_INFO = 1 << 3,
    TRPD_TBL_SEC_HASH = 1 << 4,
    TRPD_TBL_SEC_SIGN = 1 << 6,
    TRPD_TBL_PKG_AUTORUN = 1 << 7,
    
    TRPD_TBL_RESV1 = 1 << 8,
    TRPD_TBL_RESV2 = 1 << 9,
    TRPD_TBL_NOTICE = 1 << 10,
    TRPD_TBL_CHKSUM = 1 << 11 ,
    TRPD_TBL_CHRONO = 1 << 12,
    
    TRPD_TBL_RESV3 = 1 << 13,
    TRPD_TBL_DBG = 1 << 14
};
enum TRPD_UTI_LIST {
    TRPD_UTI_MANIFEST = 0xf8339907,
    TRPD_UTI_RAW_BIN = 0x2cd32674,
    TRPD_UTI_EXT_INFO = 0x22d731b8,
    TRPD_UTI_SEC_HASH = 0xd7b99bc6,
    TRPD_UTI_SEC_SIGN = 0xcbb85242,
    TRPD_UTI_PKG_AUTORUN = 0x16ea2c23,
    
    TRPD_UTI_RESV1 = 0x140f056c,
    TRPD_UTI_RESV2 = 0xaf7e92d7,
    TRPD_UTI_NOTICE = 0xf534f703,
    TRPD_UTI_CHKSUM = 0xc703f5d1,
    TRPD_UTI_CHRONO = 0xc096ff13d,
    TRPD_UTI_RESV3 = 0,
    TRPD_UTI_DBG = 0xcf6397fd
};