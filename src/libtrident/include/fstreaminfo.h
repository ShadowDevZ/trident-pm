#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
#include <sys/stat.h>
namespace LibTrident::FstreamInfo {

typedef struct {
    //todo enforce PATHMAX and use const char* to avoid unnecessary memory allocation
    std::string dirPath;
    std::string name;
    std::shared_ptr<std::fstream> hFile;
    //does not represent actual file size, but sizeof(whole file - header), not utilized yet
    std::streamsize fSize;
    IO_OpenFlag fileFlags;
    //for future use, atime
    struct stat64 fileStat;

}TRDFstreamObject;

class TrdFstreamInfo {
private:
    TRDFstreamObject xfInfo;
    bool IsOpen();
public:
    LibTrident::LTSTATUS::TridentError e;
    
   // TrdFstreamInfo() : xfInfo(std::make_shared<TRDFstreamObject>()) {};
   // TrdFstreamInfo() {
    //    xfInfo.hFile = std::make_unique<std::fstream>();
   // }
    TRDFstreamObject& GetFstreamObject() {
        return xfInfo;
        
    }
    const TRDFstreamObject& GetFstreamObject() const {
        return xfInfo;
    }
    bool SetFileStreamInfo(const TRDFstreamObject& info);
    bool CheckFileStreamInfo();
    bool CloseStream();
    
   static LibTrident::LTSTATUS::LTSTATUS StreamRemoteIsOpen(const TRDFstreamObject& info);
   static LTSTATUS::LTSTATUS CloseRemoteStream(TRDFstreamObject& info);
   static std::pair<bool, std::shared_ptr<FstreamInfo::TrdFstreamInfo>> GetFstreamContent(std::weak_ptr<FstreamInfo::TrdFstreamInfo> weakFstr);
   static LibTrident::LTSTATUS::LTSTATUS ValidateRemoteFileStreamInfo(const TRDFstreamObject& info);
};

};