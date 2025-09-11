#include "suid.h"
#include "string.h"
#include <array>
#include <string>
#include <iostream>
#include <ccattribs.h>
using namespace LibTrident;
using namespace LibTrident::UID;
constexpr std::pair<int,const char*> gTuidList [] = {
    {SUID::SUID_SECDESC,"7a153cca-f082-4837-9f8b-10905d006261"}
    
};
const char* SUID::GetSUIDString(SUID::SUIDS id) {
     for (auto&&  x: gTuidList)  {
        if (x.first == id) {
            
            return x.second;
        }
   }
   
   return "";
}

bool SUID::IsValidSUID(const char* tuid) {
    if (strnlen(tuid, SUID::SUID_MAX_LENGTH) != SUID::SUID_MAX_LENGTH) {
        return false;
    }
    for (auto&& x: gTuidList) {
        if (!strncmp(tuid, x.second, SUID::SUID_MAX_LENGTH)) {
            return true;
        }
    }
    return false;
}