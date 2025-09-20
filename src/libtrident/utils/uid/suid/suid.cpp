#include "suid.h"
#include "string.h"
#include <array>
#include <string>
#include <iostream>
#include <ccattribs.h>
#include "tstreaminfo.h"
#include "trheader.h"
#include "pkgio.h"
#include <algorithm>
#include <vector>
#include <functional>
#include <string_view>
using namespace LibTrident;
using namespace LibTrident::UID;
using namespace LibTrident::Consts::SUID;
using namespace LibTrident::Header;
using namespace LibTrident::PkgIO;
constexpr std::pair<SUID::SUIDS,const std::string_view> gTuidList [] = {
    {SUID::SUIDS::SECTION_DESCR,"7a153cca-f082-4837-9f8b-10905d006261"}
    
};
const std::string_view SUID::GetSUIDString(SUID::SUIDS id) {
     for (const auto&  x: gTuidList)  {
        if (x.first == id) {
            
            return x.second;
        }
   }
   
   return "";
}

bool SUID::IsValidSUID(const std::string_view& suid) {
    if (suid.length() != LibTrident::Consts::SUID::SUID_MAX_LENGTH) {
        return false;
    }
    for (const auto& x: gTuidList) {
        if (suid == x.second) {
            return true;
        }
    }
    return false;
}

Err::Code SUID::WriteSUIDAt(std::shared_ptr<TstreamInfo::TStreamInfo> streamInfo, std::streampos loc, SUID::SUIDS id) {  
   
   
    if (loc < 1) {
        return Err::Code::BADARG;
    }
    const std::string_view& suidString = SUID::GetSUIDString(id);
    
    if (!SUID::IsValidSUID(suidString)) {
        return Err::Code::BADARG;
    } 
    if (!streamInfo->CheckFileStreamInfo()) {
        return Err::Code::NULL_OBJ;
    }
    Err::Code errCodePresent = TRDPkgHeader::IsHeaderPresent(streamInfo);
    if (errCodePresent != Err::Code::SUCCESS) {
        return errCodePresent;
    }
   // auto& fstrInfo = streamInfo->GetFstreamObject();
    auto& fstrStream = streamInfo->GetFstreamObject().hFile;
    
    fstrStream->seekp(loc);

    if (!fstrStream) {
        
        return Err::Code::FSEEK;
    }

    //stream.WriteHeader(static_cast<const char*>(SUID), SUID::SUID_MAX_LENGTH);
    bool WriteHeaderStatus = streamInfo->WriteTStream(suidString.data(), LibTrident::Consts::SUID::SUID_MAX_LENGTH, true);
    if (!fstrStream|| !WriteHeaderStatus) {
        return Err::Code::IO_WRITE;
    }
    return Err::Code::SUCCESS;
} 
