#include "uid.h"
#include <algorithm>
#include <functional>
#include "trdconsts.h"
#include "suid.h"
#include <memory.h>
using namespace LibTrident;
//todo move to other source file
std::optional<std::vector<u64>> UID::FindStreamUIDS(const std::vector<u8> blob, const std::vector<u8>& pattern, u32 maxOccurences) {
    if (blob.empty() || pattern.empty()) {
        return std::nullopt;
        
    }
    if (maxOccurences == 0) {
        maxOccurences = UINT32_MAX;
    }

    auto algo = std::boyer_moore_horspool_searcher(pattern.begin(), pattern.end());
    auto it = blob.begin();
    u16 occurencesNow = 0;
    
    
    std::vector<u64> refs = {};
    while (it != blob.end() && occurencesNow != maxOccurences) {
        it = std::search(it, blob.end(), algo);
        if (it != blob.end() && occurencesNow != maxOccurences) {
            refs.emplace_back(it - blob.begin());
            it++;
            occurencesNow++;
        }
    }
    return refs;
    
}
std::optional<u64> UID::FindFirstUID(const std::vector<u8> blob, const std::vector<u8>& pattern) {
    if (blob.empty() || pattern.empty()) {
        return std::nullopt;
    }
    
   auto it = std::search(blob.begin(), blob.end(), 
   std::boyer_moore_horspool_searcher( pattern.begin(), pattern.end()));

    if (it != blob.end()) {
        return it - blob.begin();
    }
    return std::nullopt;
}

std::optional<std::vector<u8>> UID::GetUIDPattern(const std::string_view& str) {
    if (!LibTrident::UID::SUID::IsValidSUID(str)) {
        return std::nullopt;
    }
    std::vector<u8> pattern = {};
    pattern.reserve(Consts::SUID::SUID_MAX_LENGTH);
    for (size_t i =0; i < str.length(); ++i) {
        pattern.emplace_back(str[i]);
    }
    return pattern;

}
//when finding patterns eg. looking for SUID SECDESC string we load the whole file into dynami buffer
//later on when we add dynamic sections (the DATA table may be tens of GB's) we have to mmap the file to the memory or code chunk parser