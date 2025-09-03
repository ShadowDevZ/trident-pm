#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>


#include "trheader.h"
#include "trderr.h"
#include "pkgio.h"
#include "ioflags.h"
#include "fstreaminfo.h"
namespace LibTrident {




class TrPkg  {
public:
    
    LTSTATUS::TridentError e;
    
    //LibTrident::Header::PackageHeader hdr;
    std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fstrInfo;
    
    
    // PackageHeader pkg;
   TrPkg() : fstrInfo(std::make_shared<LibTrident::FstreamInfo::TrdFstreamInfo>()) {};
   ~TrPkg() {
    ClosePkg();
   }
   //std::shared_ptr<TRDFilStreameInfo> GetFileStreamInfo() {
   //     return fInfo;
   //     
  // }
   
   // PackageHeader hdr;
    

   
    bool OpenPackage(const std::string& path, IO_OpenFlag openFlags);
    void ClosePkg();

//private:
    
  // std::shared_ptr<TRDFilStreameInfo> fInfo;

};
};
