#pragma once
#include <cstdint>
#include "datatypes.h"
namespace LibTrident::UID {

namespace SUID {
    //maximum size of TUID, excluding NULL terminator
    constexpr u8 SUID_MAX_LENGTH = 36;
    typedef enum {
        SUID_SECDESC
        ///...
    }SUIDS;
    const char* GetSUIDString(SUID::SUIDS id);
    bool IsValidSUID(const char* tuid);



}};