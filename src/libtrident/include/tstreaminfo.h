#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
#include <sys/stat.h>
#include <optional>
#include "ioflags.h"
namespace LibTrident::TstreamInfo {

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

class TStreamInfo{
private:
    TRDFstreamObject xfInfo;
public:
    LibTrident::Err::TridentError e;
    
    // TStreamInfo() : xfInfo(std::make_shared<TRDFstreamObject>()) {};
    // TStreamInfo() {
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
    
    template <typename T>
    bool WriteTStream(const T& t, u64 size, bool increment=true) {
        return WriteTStream(reinterpret_cast<const char*>(&t), size, increment);
    }
    template <typename T>
    bool WriteTStream(T& t, bool increment=true) {
        return WriteTStream(t, sizeof(t), increment);
    }
    
    bool WriteTStream(const char* data, u64 size, bool increment=true);
    
    
    
    template <typename T>
    bool ReadTStream(T& t, u64 size) {
        return ReadTStream(reinterpret_cast<char*>(&t), size);
    }
    
    template <typename T>
    bool ReadTStream(T& t) {
        return ReadTStream(t, sizeof(t));
    }


    bool ReadTStream(char* s, u64 size);
  // static LibTrident::Err::Code StreamRemoteIsOpen(const TRDFstreamObject& info);
  // static LibTrident::Err::Code CloseRemoteStream(TRDFstreamObject& info);
   static std::optional<std::shared_ptr<TstreamInfo::TStreamInfo>> GetFstreamContent(std::weak_ptr<TstreamInfo::TStreamInfo> weakFstr);
  // static LibTrident::Err::Code ValidateRemoteFileStreamInfo(const TRDFstreamObject& info);
};

};