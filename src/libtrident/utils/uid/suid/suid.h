#pragma once
#include <cstdint>
#include "datatypes.h"
#include "trdconsts.h"
#include "tstreaminfo.h"
#include "uid.h"

namespace LibTrident::UID {

namespace SUID {
    //maximum size of TUID, excluding NULL terminator
    
    enum class SUIDS{
        SECTION_DESCR
        ///...
    };
    const std::string_view GetSUIDString(SUID::SUIDS id);
    bool IsValidSUID(const std::string_view& suid);
    LibTrident::Err::Code WriteSUIDAt(std::shared_ptr<TstreamInfo::TStreamInfo> streamInfo, std::streampos loc, SUID::SUIDS id);
    std::pair<bool,std::vector<u8>> GetUIDPattern(const char* str);


}};