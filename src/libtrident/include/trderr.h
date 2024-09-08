#pragma once
#include <stdint.h>
typedef uint32_t trderr_t;
#include <stdio.h>
#include "libtrident.h"


#define _TRD_MERR_STR(x) #x 
trderr_t TRD_GetLastError();
bool TRD_SetLastError(trderr_t err);
const char* TRD_TranslateError(trderr_t err);

typedef enum {
    TRDE_SUCCESS = 0,
    TRDE_FAILURE = 1,
    TRDE_BAD_ARG = 2,
    TRDE_UNSPECIFIED = 3,
    TRDE_MALLOC_FAIL = 4,
    TRDE_IO_FAIL = 5,
    TRDE_BAD_SEGMENT = 6,
    TRDE_INV_CHKSUM = 7,
    TRDE_BAD_OFFSET = 8,
    TRDE_BAD_HDR = 9,
    TRDE_DB_LOCKED = 10,
    TRDE_IO_ACCESS = 11,
    TRDE_FCB_NONE = 12,
    TRDE_NETW_FAIL = 13,
    TRDE_REMOTE_DENY = 14,
    TRDE_RES_NOT_FOUND = 15,
    TRDE_TOKEN_EMPTY = 16,
    TRDE_TOKEN_INV = 17,
    TRDE_REMOTE_TIMEOUT = 18,
    TRDE_VER_MISSMATCH = 19,
    TRDE_FMT_OUTDATED = 20,
    TRDE_FMT_UNSUPPORTED = 21,
    TRDE_FILE_CORRUPTED = 22,
    TRDE_XMLPARSE = 23,
    TRDE_NOFILE = 24,
    TRDE_NULL = 25

}TRD_ERR_CODES;