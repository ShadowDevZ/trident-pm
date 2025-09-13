#pragma once
#include "ccattribs.h"
#include "datatypes.h"
#include <vector>


namespace LibTrident::UID {
    std::pair<bool,std::vector<u8>> GetUIDPattern(const char* str);
    u64 FindFirstPattern(const std::vector<u8> blob, const std::vector<u8>& pattern);
    std::vector<u64> FindPatterns(const std::vector<u8> blob, const std::vector<u8>& pattern, u32 maxOccurences=0);
}