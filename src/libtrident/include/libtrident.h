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
    ClosePkg();
   }
   TRDPkgHeader header();
   const LibTrident::Tstream::TStreamInfo& GetTstream() const {
    return fstrInfo;
   }
   LibTrident::Tstream::TStreamInfo& GetTstream(){
    return fstrInfo;
   }

   explicit TrPkg(const std::filesystem::path& path,const IOFLAGS::TRDAccessModel& access)  {
    OpenPackage(path, access);
   }
   explicit TrPkg(const std::filesystem::path& path, IOFLAGS::TrdOpenIO open, IOFLAGS::TrdAccessIO access, 
    IOFLAGS::TrdXattrIO xattr = IOFLAGS::TrdXattrIO::None)  {
    
      OpenPackage(path, {open, access, xattr, IOFLAGS::_TrdInternalIO::None});
   }

  void OpenPackage(const std::filesystem::path& path, const IOFLAGS::TRDAccessModel& accessModel);
  void ClosePkg();



private:
  LibTrident::Tstream::TStreamInfo fstrInfo {};
  LibTrident::TRD_HEADER trdHdr {};

  //TRDPkgHeader headerSection;
  friend class TRDPkgHeader;
  // std::shared_ptr<TRDFstreamObject> fInfo;

};
};
