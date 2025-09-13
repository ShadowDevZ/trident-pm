#include "suid.h"
#include "string.h"
#include <array>
#include <string>
#include <iostream>
#include <ccattribs.h>
#include "fstreaminfo.h"
#include "trheader.h"
#include "pkgio.h"
#include <algorithm>
#include <vector>
#include <functional>
using namespace LibTrident;
using namespace LibTrident::UID;
using namespace LibTrident::Consts::SUID;
using namespace LibTrident::Header;
using namespace LibTrident::PkgIO;
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
    if (strnlen(tuid, LibTrident::Consts::SUID::SUID_MAX_LENGTH) != LibTrident::Consts::SUID::SUID_MAX_LENGTH) {
        return false;
    }
    for (auto&& x: gTuidList) {
        if (!strncmp(tuid, x.second, LibTrident::Consts::SUID::SUID_MAX_LENGTH)) {
            return true;
        }
    }
    return false;
}

LTSTATUS::LTSTATUS SUID::WriteSUIDAt(std::shared_ptr<FstreamInfo::TrdFstreamInfo> streamInfo, std::streampos loc, SUID::SUIDS id) {  
   
   // auto [checkWeakRef, sharedPtr] = FstreamInfo::TrdFstreamInfo::GetFstreamContent(wFstr);
   // if (!checkWeakRef) {
   //     e.SetError(LTSTATUS::IREF_EXPIRED);
   //     return false;
    if (loc < 1) {
        return LTSTATUS::BADARG;
    }
    const char* suidString = SUID::GetSUIDString(id);
    if (!SUID::IsValidSUID(suidString)) {
        return LTSTATUS::BADARG;
    } 
    if (!streamInfo->CheckFileStreamInfo()) {
        return LTSTATUS::NULL_OBJ;
    }
    LTSTATUS::LTSTATUS errCodePresent = TRDPkgHeader::IsHeaderPresent(streamInfo);
    if (errCodePresent != LTSTATUS::SUCCESS) {
        return errCodePresent;
    }
    auto& fstrInfo = streamInfo->GetFstreamObject();
    auto& fstrStream = fstrInfo.hFile;
    
    fstrStream->seekp(loc);

    if (!fstrStream) {
        
        return LTSTATUS::FSEEK;
    }

    //stream.WriteHeader(static_cast<const char*>(SUID), SUID::SUID_MAX_LENGTH);
    bool WriteHeaderStatus = PkgIO::FileOperations::WriteLeStream(fstrInfo, suidString, LibTrident::Consts::SUID::SUID_MAX_LENGTH, true);
    if (!fstrStream|| !WriteHeaderStatus) {
        return LTSTATUS::IO_WRITE;
    }
    return LTSTATUS::SUCCESS;
} 
