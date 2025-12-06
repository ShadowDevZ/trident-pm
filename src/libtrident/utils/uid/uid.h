#pragma once
#include "ccattribs.h"
#include "datatypes.h"
#include <vector>
#include <optional>
#include <string_view>
namespace LibTrident::UID {
    std::optional<std::vector<u8>> GetUIDPattern(const std::string_view str);
    std::optional<u64> FindFirstUID(const std::vector<u8> blob, const std::vector<u8>& pattern);
    std::optional<std::vector<u64>> FindStreamUIDS(const std::vector<u8> blob, const std::vector<u8>& pattern, u32 maxOccurences=0);
}