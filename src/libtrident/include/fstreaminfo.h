#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
namespace LibTrident::FstreamInfo {

typedef struct {
    std::string dirPath;
    std::string name;
    std::shared_ptr<std::fstream> hFile;
    std::streampos seekOffsetRead;
    std::streampos seekOffsetWrite;
    //does not represent actual file size, but sizeof(whole file - header)
    std::streamsize fSize;
    IO_OpenFlag fileFlags;

}TRDFilStreameInfo;

class TrdFstreamInfo {
private:
    TRDFilStreameInfo xfInfo;
    bool IsOpen();
public:
    LibTrident::LTSTATUS::TridentError e;
    
   // TrdFstreamInfo() : xfInfo(std::make_shared<TRDFilStreameInfo>()) {};
   // TrdFstreamInfo() {
    //    xfInfo.hFile = std::make_unique<std::fstream>();
   // }
    TRDFilStreameInfo& GetFileStreamInfo() {
        return xfInfo;
        
    }
    bool SetFileStreamInfo(const TRDFilStreameInfo& info);
    bool CheckFileStreamInfo();
    bool CloseStream();
    
   static LibTrident::LTSTATUS::LTSTATUS StreamRemoteIsOpen(const TRDFilStreameInfo& info);
   static LTSTATUS::LTSTATUS CloseRemoteStream(TRDFilStreameInfo& info);
   static std::pair<bool, std::shared_ptr<FstreamInfo::TrdFstreamInfo>> GetFstreamContent(std::weak_ptr<FstreamInfo::TrdFstreamInfo> weakFstr);
   static LibTrident::LTSTATUS::LTSTATUS ValidateRemoteFileStreamInfo(const TRDFilStreameInfo& info);
};

};