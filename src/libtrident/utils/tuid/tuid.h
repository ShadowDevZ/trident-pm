#pragma once
#include <cstdint>
#include "datatypes.h"
namespace LibTrident::TUID {
    //maximum size of TUID, excluding NULL terminator
    constexpr u8 TUID_MAX_LENGTH = 38;
    typedef enum {
        TUID_SECDESC
        ///...
    }TUIDS;
    const char* GetTUIDString(TUID::TUIDS id);
    bool IsValidTUID(const char* tuid);

};