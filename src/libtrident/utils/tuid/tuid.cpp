#include "tuid.h"
#include "string.h"
#include <array>
#include <string>
#include <iostream>
#include <ccattribs.h>
using namespace LibTrident;

constexpr std::pair<int,const char*> gTuidList [] = {
    {TUID::TUID_SECDESC,"{7a153cca-f082-4837-9f8b-10905d006261}"}
    
};
const char* TUID::GetTUIDString(TUID::TUIDS id) {
     for (auto&&  x: gTuidList)  {
        if (x.first == id) {
            
            return x.second;
        }
   }
   
   return "";
}

bool TUID::IsValidTUID(const char* tuid) {
    if (strnlen(tuid, TUID::TUID_MAX_LENGTH) != TUID::TUID_MAX_LENGTH) {
        return false;
    }
    for (auto&& x: gTuidList) {
        if (!strncmp(tuid, x.second, TUID::TUID_MAX_LENGTH)) {
            return true;
        }
    }
    return false;
}