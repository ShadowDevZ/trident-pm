//temporarily disabled for testing
/*
#include "suid.h"
#include "string.h"
#include <array>
#include <string>
#include <iostream>
#include <ccattribs.h>
#include "tstreaminfo.h"
#include "trheader.h"
#include "binarySerializer.h"
#include <algorithm>
#include <vector>
#include <functional>
#include <string_view>
using namespace LibTrident;
using namespace LibTrident::UID;
using namespace LibTrident::Consts::SUID;
using namespace LibTrident::PkgIO;
constexpr std::pair<SUID::SUIDS,const std::string_view> gTuidList [] = {
    //SUIDS are in following format (XXX-UUID) where XXX is shorthand name
    {SUID::SUIDS::SECTION_DESCR,"SDR-7a153cca-f082-4837-9f8b-10905d006261"}
    
};
const std::optional<std::string_view> SUID::GetSUIDString(SUID::SUIDS id) {
     for (const auto&  x: gTuidList)  {
        if (x.first == id) {
            
            return x.second;
        }
   }
   
   return std::nullopt;
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

std::expected<void, Err::TrdError> SUID::WriteSUIDAt(std::shared_ptr<Tstream::TStreamInfo> streamInfo, std::streampos loc, SUID::SUIDS id) {  
   
   
    if (loc < 1) {
        return std::unexpected(Err::TrdError(Err::Code::InvalidFuncArg));
    }
    auto haveSuid = SUID::GetSUIDString(id);
    if (!haveSuid.has_value()) {
        return std::unexpected(Err::TrdError(Err::Code::BadObject));
    }
    const std::string_view suidString = haveSuid.value();
    
    if (!SUID::IsValidSUID(suidString)) {
        return std::unexpected(Err::TrdError(Err::Code::InvalidFuncArg));
    } 
    auto strInfo = streamInfo->CheckFileStreamInfo();
    if (!strInfo.has_value()) {
        return std::unexpected(strInfo.error());
    }
    auto hdrPresent = TRDPkgHeader::IsHeaderPresent(streamInfo);
    if (!hdrPresent.has_value()) {
        return std::unexpected(hdrPresent.error());
    }
   // auto& fstrInfo = streamInfo->GetFstreamObject();
   
    
    streamInfo->SetSeekPosW(loc);
    


    //stream.WriteHeader(static_cast<const char*>(SUID), SUID::SUID_MAX_LENGTH);
    streamInfo->WriteTStream(suidString.data(), LibTrident::Consts::SUID::SUID_MAX_LENGTH, true);
    return {};
} 

*/