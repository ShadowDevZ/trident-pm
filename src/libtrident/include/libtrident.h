/**
 * @file libtrident.h
 * @brief Main header to include
 * 
 * 
 */
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
   /// @brief provides API to manipulate the file header
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
   /**
    * @brief Opens the TRPX package
    * 
    * @param path absolute or relative path to the package
    * @param accessModel additional flags to define access
    */
  void openPackage(const std::filesystem::path& path, const TRDAccessModel& accessModel);
  /**
   * @brief Closes the package. No need to call this because of RAII
   * 
   */
  void closePkg();



private:
  LibTrident::Impl::TStreamInfo fstrInfo {};
  LibTrident::TRD_HEADER trdHdr {};

  //TRDPkgHeader headerSection;
  friend class TRDPkgHeader;
  // std::shared_ptr<TRDFstreamObject> fInfo;

};
};
