#pragma once
#include "ccattribs.h"
#include "datatypes.h"
#include <vector>
#include <optional>
#include <string_view>
namespace Trd::UID {
    std::optional<std::vector<u8>> GetUIDPattern(std::string_view str);
    std::optional<u64> FindFirstUID(std::vector<u8> blob, const std::vector<u8>& pattern);
    std::optional<std::vector<u64>> FindStreamUIDS(std::vector<u8> blob, const std::vector<u8>& pattern, u32 maxOccurences=0);
}