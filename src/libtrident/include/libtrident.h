#pragma once
#include <cstdint>
#include <cstddef>
#include <string>

#include "hrddefs.h"

#include "trderr.h"
#include "pkgio.h"
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

   const LibTrident::Tstream::TStreamInfo& getTstream() const {
        return fstrInfo;
   }
   LibTrident::Tstream::TStreamInfo& getTstream(){
        return fstrInfo;
   }

   explicit TrPkg(const std::filesystem::path& path,const IOFLAGS::TRDAccessModel& access)  {
        openPackage(path, access);
   }
   explicit TrPkg(const std::filesystem::path& path, IOFLAGS::TrdOpenIO open, IOFLAGS::TrdAccessIO access, 
    IOFLAGS::TrdXattrIO xattr = IOFLAGS::TrdXattrIO::None)  {
    
        openPackage(path, {open, access, xattr, IOFLAGS::_TrdInternalIO::None});
   }

  void openPackage(const std::filesystem::path& path, const IOFLAGS::TRDAccessModel& accessModel);
  void closePkg();



private:
  LibTrident::Tstream::TStreamInfo fstrInfo {};
  LibTrident::TRD_HEADER trdHdr {};

  //TRDPkgHeader headerSection;
  friend class TRDPkgHeader;
  // std::shared_ptr<TRDFstreamObject> fInfo;

};
};
