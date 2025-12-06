#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>


#include "trheader.h"
#include "trderr.h"
#include "pkgio.h"
#include "ioflags.h"
#include "tstreaminfo.h"
#include <filesystem>

namespace LibTrident {




class TrPkg  {
public:
    
    //LibTrident::Header::PackageHeader hdr;
    std::shared_ptr<LibTrident::Tstream::TStreamInfo> fstrInfo;
    
    
    // PackageHeader pkg;
   TrPkg() : fstrInfo(std::make_shared<LibTrident::Tstream::TStreamInfo>()) {};
   ~TrPkg() {
    dbgprintf("~Destructor called\n");
    ClosePkg();
   }

   TrPkg(const std::filesystem::path& path,const IOFLAGS::TRDAccessModel& access) : 
   fstrInfo(std::make_shared<LibTrident::Tstream::TStreamInfo>()) {


    OpenPackage(path, access);
   }
   TrPkg(const std::filesystem::path& path, IOFLAGS::TrdOpenIO open, IOFLAGS::TrdAccessIO access, 
    IOFLAGS::TrdXattrIO xattr = IOFLAGS::TrdXattrIO::None): fstrInfo(std::make_shared<LibTrident::Tstream::TStreamInfo>()) {
    
      OpenPackage(path, {open, access, xattr, IOFLAGS::_TrdInternalIO::None});
   }
   //std::shared_ptr<TRDFstreamObject> GetFstreamObject() {
   //     return fInfo;
   //     
  // }
   
   // PackageHeader hdr;
    

   
    void OpenPackage(const std::filesystem::path& path, const IOFLAGS::TRDAccessModel& accessModel);
    void ClosePkg();

//private:
 
  // std::shared_ptr<TRDFstreamObject> fInfo;

};
};
