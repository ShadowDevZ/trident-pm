#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>


#include "trheader.h"
#include "trderr.h"
#include "pkgio.h"
#include "ioflags.h"
#include "fstreaminfo.h"
#include <filesystem>
namespace LibTrident {




class TrPkg  {
public:
    
    LibTrident::Err::TridentError e;
    
    //LibTrident::Header::PackageHeader hdr;
    std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fstrInfo;
    
    
    // PackageHeader pkg;
   TrPkg() : fstrInfo(std::make_shared<LibTrident::FstreamInfo::TrdFstreamInfo>()) {};
   ~TrPkg() {
    dbgprintf("~Destructor called\n");
    ClosePkg();
   }

   TrPkg(const std::filesystem::path& path, u16 openFlags) : fstrInfo(std::make_shared<LibTrident::FstreamInfo::TrdFstreamInfo>()) {
    OpenPackage(path, openFlags);
   }
   //std::shared_ptr<TRDFstreamObject> GetFstreamObject() {
   //     return fInfo;
   //     
  // }
   
   // PackageHeader hdr;
    

   
    void OpenPackage(const std::filesystem::path& path, IO_OpenFlag openFlags);
    void ClosePkg();

//private:
 
  // std::shared_ptr<TRDFstreamObject> fInfo;

};
};
