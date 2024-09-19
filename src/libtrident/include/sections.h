#pragma once
#include "trheader.h"
#include "trderr.h"

trderr_t TRD_GenerateSectionHeader(_TRD_PKGI* pkg, uint16_t tablesMax);
trderr_t TRD_GetSectionDescriptor(_TRD_PKGI* pkg, TRD_SECTION_DESCRIPTOR* tsd);