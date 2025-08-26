#pragma once
#include "libtrident.h"

PACKED_STRUCT {
    int a;

}TRD_HEADER;



class Hdr {
public:
    LTSTATUS::TridentError e;
    bool WriteHeader(TRD_HEADER& hdrIn);
    bool ReadHeader(TRD_HEADER& hdrOut);
    bool UpdateHeader(TRD_HEADER& hdrInfo);
    bool Sync(TRD_HEADER& hdrOut);
    bool ValidateHeader(TRD_HEADER& hdrOut);

    LTSTATUS::LTSTATUS lastErr;
private:
TRD_HEADER cacheHdr;
};