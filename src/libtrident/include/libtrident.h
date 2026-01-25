#pragma once
#include <cstdint>
#include <cstddef>
#include <string>

#include "hrddefs.h"

#include "trderr.h"
#include "binarySerializer.h"
#include "ioflags.h"
#include "tstreaminfo.h"
#include <filesystem>

namespace LibTrident {
     

class TRDPkgHeader;

class TrPkg  {
public:
  
   TrPkg()  {};

   ~TrPkg() {
      dbgprintf("~Destructor called\n");
      closePkg();
   }

   TRDPkgHeader header();

   const LibTrident::Impl::TStreamInfo& getTstream() const {
        return fstrInfo;
   }
   LibTrident::Impl::TStreamInfo& getTstream(){
        return fstrInfo;
   }

   explicit TrPkg(const std::filesystem::path& path,const TRDAccessModel& access)  {
        openPackage(path, access);
   }
   explicit TrPkg(const std::filesystem::path& path, TrdOpenIO open, TrdAccessIO access, 
    TrdXattrIO xattr = TrdXattrIO::None)  {
    
        openPackage(path, {open, access, xattr, _TrdInternalIO::None});
   }

  void openPackage(const std::filesystem::path& path, const TRDAccessModel& accessModel);
  void closePkg();



private:
  LibTrident::Impl::TStreamInfo fstrInfo {};
  LibTrident::TRD_HEADER trdHdr {};

  //TRDPkgHeader headerSection;
  friend class TRDPkgHeader;
  // std::shared_ptr<TRDFstreamObject> fInfo;

};
};
