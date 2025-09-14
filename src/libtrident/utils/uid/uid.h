#pragma once
#include "ccattribs.h"
#include "datatypes.h"
#include <vector>
#include <optional>

namespace LibTrident::UID {
    std::optional<std::vector<u8>> GetUIDPattern(const char* str);
    std::optional<u64> FindFirstUID(const std::vector<u8> blob, const std::vector<u8>& pattern);
    std::optional<std::vector<u64>> FindStreamUIDS(const std::vector<u8> blob, const std::vector<u8>& pattern, u32 maxOccurences=0);
}