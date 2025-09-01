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
   //std::shared_ptr<TRDFilStreameInfo> GetFileStreamInfo() {
   //     return fInfo;
   //     
  // }
   
   // PackageHeader hdr;
    

   
    bool OpenPackage(std::string path, IO_OpenFlag openFlags);

//private:
    
  // std::shared_ptr<TRDFilStreameInfo> fInfo;

};
};
