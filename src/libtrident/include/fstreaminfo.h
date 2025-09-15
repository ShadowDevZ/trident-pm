#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
#include <sys/stat.h>
#include <optional>
#include "ioflags.h"
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
    bool fileOpened;

}TRDFstreamObject;

class TrdFstreamInfo {
private:
    TRDFstreamObject xfInfo;
    public:
    LibTrident::Err::TridentError e;
    
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
    //checks if each field is correctly set
    bool CheckFileStreamInfo();
    bool CloseStream();
    //checks if the stream is only MARKED as open, data may be missing or corrupted
    bool IsOpen();
    
   static LibTrident::Err::Code StreamRemoteIsOpen(const TRDFstreamObject& info);
   static LibTrident::Err::Code CloseRemoteStream(TRDFstreamObject& info);
   static std::optional<std::shared_ptr<FstreamInfo::TrdFstreamInfo>> GetFstreamContent(std::weak_ptr<FstreamInfo::TrdFstreamInfo> weakFstr);
   static LibTrident::Err::Code ValidateRemoteFileStreamInfo(const TRDFstreamObject& info);
};

};