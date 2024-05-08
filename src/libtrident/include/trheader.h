#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "libtrident.h"
#define TRD_PKG_MAGIC {'T', 'R', '.', 'P', 'K', 'G'}

typedef struct{
    uint8_t Major, Minor, Revision;
}TRPD_PKG_VERSION;

typedef struct {
    char magic[5];
    uint16_t ver;
    uint8_t padding;

}__TRIDENT_PACKED TRPD_HEADER;

