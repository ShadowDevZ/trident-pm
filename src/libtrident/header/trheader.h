#pragma once

#include "trderr.h"
#include "datatypes.h"
#include "ccattribs.h"
#include "tstreaminfo.h"
#include "trdconsts.h"
#include "sectioncommon.h"
#include <expected>
#include "hrddefs.h"
namespace LibTrident {
    


class TrPkg;
//todo each SECTION should inherit from something like SectionCommon, standardize the functions
class TRDPkgHeader /*final : public LibTrident::Sections::SectionCommon<TRD_HEADER> */  {
public:
    

    std::expected<void, LibTrident::Err::TrdError> write() /*override*/;
    std::expected<void, LibTrident::Err::TrdError> read() /*override*/;
    static std::expected<TRD_HEADER, LibTrident::Err::TrdError> readBack(Tstream::TStreamInfo& tStream)/*override*/;
    bool isValid() /*override*/;
    
    const TRD_HEADER& getHeader() const;
    
    std::expected<void, LibTrident::Err::TrdError> create(u32 buildFlgs, u8 archType, u8 comprType = COMMPRALG_NONE);
    std::expected<void, LibTrident::Err::TrdError> create(const TRD_HDRFIELD_UPDATE& field);
    std::expected<void, LibTrident::Err::TrdError> updateHeader(const TRD_HDRFIELD_UPDATE& update);
    std::expected<void, LibTrident::Err::TrdError> updateIoctrlProp(u16 ioctrl);
    std::expected<void, LibTrident::Err::TrdError> updateFileLenProp(u64 len);
    
    
    static std::expected<u16, LibTrident::Err::TrdError> formatHeaderVersion(u8 major, u8 minor, u8 revision);
    static std::expected<std::string, LibTrident::Err::TrdError> headerVersionFormatToString(u16 fmt, bool abRevision=true);
    
    static std::expected<void, LibTrident::Err::TrdError> isHeaderPresent(LibTrident::Tstream::TStreamInfo& fStreamInfo);
    
    explicit TRDPkgHeader(TrPkg& pkg) : trpkg(pkg) {};
    
    
private:
    friend class TrPkg;
    TrPkg& trpkg;
    
   
    static std::expected<void, LibTrident::Err::TrdError> ICheckCRC(u32 crc, const TRD_HEADER& hdr);
    static bool ICheckHeaderSize(const TRD_HEADER& hdr);
    static std::expected<void, LibTrident::Err::TrdError> IValidateHeader(const TRD_HEADER& hdrIn);
    //std::pair<bool,Tstream::TRDFstreamObject&> ICheckAndGetFstreamContent();
    
    
    //static u16 FormatHeaderVersion(u8 major, u8 minor, u8 revision);
    
};
};