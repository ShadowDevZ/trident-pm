#pragma once
#include "dyntbl.h"
#include <stdint.h>
typedef uint32_t CRC32;
CRC32 UpdateTableChecksum(CRC32 rcrc, TRD_DYNTBL_META* meta, void* data, uint32_t size);