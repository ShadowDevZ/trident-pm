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
    std::streamsize fSize;
    IO_OpenFlag fileFlags;

}TRDFilStreameInfo;

class TrdFstreamInfo {
private:
    std::shared_ptr<TRDFilStreameInfo> xfInfo;
public:
    LibTrident::LTSTATUS::TridentError e;

    TrdFstreamInfo() : xfInfo(std::make_shared<TRDFilStreameInfo>()) {};
    std::shared_ptr<TRDFilStreameInfo> GetFileStreamInfo() {
        return xfInfo;
        
   }
   bool CheckFileStreamInfo();
   static LibTrident::LTSTATUS::LTSTATUS ValidateFileStreamInfo(TRDFilStreameInfo& info);
   bool SetFileStreamInfo(TRDFilStreameInfo& info);
  
   
};

};