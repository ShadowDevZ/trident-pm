#pragma once
#include <cstdint>
#include "datatypes.h"
#include "trdconsts.h"
#include "fstreaminfo.h"
#include "uid.h"

namespace LibTrident::UID {

namespace SUID {
    //maximum size of TUID, excluding NULL terminator
    
    typedef enum {
        SUID_SECDESC
        ///...
    }SUIDS;
    const char* GetSUIDString(SUID::SUIDS id);
    bool IsValidSUID(const char* tuid);
    LTSTATUS::LTSTATUS WriteSUIDAt(std::shared_ptr<FstreamInfo::TrdFstreamInfo> streamInfo, std::streampos loc, SUID::SUIDS id);
    std::pair<bool,std::vector<u8>> GetUIDPattern(const char* str);


}};