#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
namespace LibTrident::FstreamInfo {

class TrdFstreamInfo {
private:
    std::shared_ptr<TRDFilStreameInfo> xfInfo;
public:
    LibTrident::LTSTATUS::TridentError e;

    TrdFstreamInfo() : xfInfo(std::make_shared<TRDFilStreameInfo>()) {};
    std::shared_ptr<TRDFilStreameInfo> GetFileStreamInfo() {
        return xfInfo;
        
   }
   static LibTrident::LTSTATUS::LTSTATUS ValidateFileStreamInfo(TRDFilStreameInfo& info);
   bool SetFileStreamInfo(TRDFilStreameInfo& info);
   bool SetFileStreamInfo(std::shared_ptr<TRDFilStreameInfo> info);
};

};