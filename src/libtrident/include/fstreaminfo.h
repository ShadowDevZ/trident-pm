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
    std::shared_ptr<TRDFilStreameInfo> xfInfo;
    bool IsOpen();
    public:
    LibTrident::LTSTATUS::TridentError e;
    
    TrdFstreamInfo() : xfInfo(std::make_shared<TRDFilStreameInfo>()) {};
    std::shared_ptr<TRDFilStreameInfo> GetFileStreamInfo() {
        return xfInfo;
        
    }
    bool CheckFileStreamInfo();
    bool CloseStream();
    
   static LibTrident::LTSTATUS::LTSTATUS StreamIsOpen(const TRDFilStreameInfo& info);
   static LibTrident::LTSTATUS::LTSTATUS ValidateFileStreamInfo(const TRDFilStreameInfo& info);
   bool SetFileStreamInfo(TRDFilStreameInfo& info);
  
   
};

};