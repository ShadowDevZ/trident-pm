/**
 * @file trheader.h
 * @brief TRPX header file class
 * 
 * 
 */
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
    
    /**
     * @brief Write the internal header to the file
     * 
     * @return std::expected<void, LibTrident::Err::TrdError> 
     */
    std::expected<void, LibTrident::Err::TrdError> write() /*override*/;

    /**
     * @brief Reads the file header. To access this instance call getHeader()
     * 
     * @return std::expected<void, LibTrident::Err::TrdError> 
     */
    std::expected<void, LibTrident::Err::TrdError> read() /*override*/;
    /**
     * @brief Reads the header from the file and returns the copy without altering the internal header
     * 
     * @param tStream reference to the valid TStream
     * @return std::expected<TRD_HEADER, LibTrident::Err::TrdError> if header is present returns
     * the valid and deserialized Header otherwise provides additional error info.
     */
    static std::expected<TRD_HEADER, LibTrident::Err::TrdError> readBack(Impl::TStreamInfo& tStream)/*override*/;
    bool isValid() /*override*/;
    /**
     * @brief Retrieves the references to the internal header
     * 
     * @return const TRD_HEADER&
     */
    const TRD_HEADER& getHeader() const;
    /**
     * @brief Creates new header file and assigns but does not writes it
     * 
     * @param buildFlgs build flags to indicate how the package was built
     * @param archType  type of supported architecture
     * @param comprType compression algorithm hint
     * @return std::expected<void, LibTrident::Err::TrdError> 
     */
    std::expected<void, LibTrident::Err::TrdError> create(BuildFlags::Flags buildFlgs, ArchType::Type archType, 
                                                         GlobalCompression::Algorithm comprType = GlobalCompression::None);
    std::expected<void, LibTrident::Err::TrdError> create(const TRD_HDRFIELD_UPDATE& field);
    /**
     * @brief Updates the available header fields
     * @todo make the fields optional to get riod of other update functions
     * @param update data to update
     * @return std::expected<void, LibTrident::Err::TrdError> 
     */
    std::expected<void, LibTrident::Err::TrdError> updateHeader(const TRD_HDRFIELD_UPDATE& update);
    std::expected<void, LibTrident::Err::TrdError> updateIoctrlProp(PackageIOCtrl::Flag ioctrl);
    std::expected<void, LibTrident::Err::TrdError> updateFileLenProp(u64 len);
    
    /**
     * @brief Returns the properly formatted header version
     * 
     * @param major major header version
     * @param minor minor header version
     * @param revision header revisioon
     * @return std::expected<u16, LibTrident::Err::TrdError> formatted version
     */
    static std::expected<u16, LibTrident::Err::TrdError> formatHeaderVersion(u8 major, u8 minor, u8 revision);
    static std::expected<std::string, LibTrident::Err::TrdError> headerVersionFormatToString(u16 fmt, bool abRevision=true);
    /**
     * @brief Checks whether file has already written header
     * 
     * @param fStreamInfo reference to TStream
     * @return std::expected<void, LibTrident::Err::TrdError> 
     */
    static std::expected<void, LibTrident::Err::TrdError> isHeaderPresent(LibTrident::Impl::TStreamInfo& fStreamInfo);
    
    explicit TRDPkgHeader(TrPkg& pkg) : trpkg(pkg) {};
    
    
private:
    friend class TrPkg;
    TrPkg& trpkg;
    
   
    static std::expected<void, LibTrident::Err::TrdError> ICheckCRC(const TRD_HEADER& hdr);
    static bool ICheckHeaderSize(const TRD_HEADER& hdr);
    static std::expected<void, LibTrident::Err::TrdError> IValidateHeader(const TRD_HEADER& hdrIn);
    //std::pair<bool,Tstream::TRDFstreamObject&> ICheckAndGetFstreamContent();
    
    
    //static u16 FormatHeaderVersion(u8 major, u8 minor, u8 revision);
    
};
};