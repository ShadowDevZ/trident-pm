/*
#pragma once
#include <cstdint>
#include "datatypes.h"
#include "trdconsts.h"
#include "tstreaminfo.h"
#include "uid.h"
#include <optional>
namespace Trd::UID {

namespace SUID {
    //maximum size of TUID, excluding NULL terminator
    
    enum class SUIDS{
        SECTION_DESCR
        ///...
    };
    const std::optional<std::string_view> GetSUIDString(SUID::SUIDS id);
    bool IsValidSUID(std::string_view suid);
    std::expected<void, Err::TrdError> WriteSUIDAt(std::shared_ptr<Impl::TStreamInfo> streamInfo, std::streampos loc, SUID::SUIDS id);
    std::optional<std::vector<u8>> GetUIDPattern(std::string_view str);


}};
*/