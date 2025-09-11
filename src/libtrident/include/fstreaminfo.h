#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
namespace LibTrident::FstreamInfo {

typedef struct {
    std::string dirPath;
    std::string name;
    std::shared_ptr<std::fstream> hFile;
    std::streampos seekOffsetReadHeader;
    std::streampos seekOffsetWriteHeader;
    //does not represent actual file size, but sizeof(whole file - header)
    std::streamsize fSize;
    IO_OpenFlag fileFlags;

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