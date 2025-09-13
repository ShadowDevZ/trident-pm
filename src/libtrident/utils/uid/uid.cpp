#include "uid.h"
#include <algorithm>
#include <functional>
#include "trdconsts.h"
#include "suid.h"
#include <memory.h>
using namespace LibTrident;
//todo move to other source file
std::vector<u64> UID::FindPatterns(const std::vector<u8> blob, const std::vector<u8>& pattern, u32 maxOccurences) {
    std::vector<u64> refs = {};
    if (blob.empty() || pattern.empty()) {
        return refs;
        
    }
    if (maxOccurences == 0) {
        maxOccurences = UINT32_MAX;
    }

    auto algo = std::boyer_moore_horspool_searcher(pattern.begin(), pattern.end());
    auto it = blob.begin();
    u16 occurencesNow = 0;


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
u64 UID::FindFirstPattern(const std::vector<u8> blob, const std::vector<u8>& pattern) {
    if (blob.empty() || pattern.empty()) {
        return UINT64_MAX;
        
    }
    
   auto it = std::search(blob.begin(), blob.end(), 
   std::boyer_moore_horspool_searcher( pattern.begin(), pattern.end()));

    if (it != blob.end()) {
        return it - blob.begin();
    }
    return UINT64_MAX;
}

std::pair<bool,std::vector<u8>> UID::GetUIDPattern(const char* str) {
    std::vector<u8> pattern = {};
    pattern.reserve(Consts::SUID::SUID_MAX_LENGTH);
    if (!str || !LibTrident::UID::SUID::IsValidSUID(str)) {
        return {false, pattern};
    }
    for (size_t i =0; i < strnlen(str, Consts::SUID::SUID_MAX_LENGTH); ++i) {
        pattern.emplace_back(str[i]);
    }
    return {true, pattern};

}
//when finding patterns eg. looking for SUID SECDESC string we load the whole file into dynami buffer
//later on when we add dynamic sections we have to mmap the file to the memory or code chunk parser