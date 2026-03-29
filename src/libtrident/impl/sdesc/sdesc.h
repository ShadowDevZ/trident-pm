#pragma once

#include "tstreaminfo.h"
#include "binarySerializer.h"
#include "sdescdefs.h"
#include "trderr.h"
#include "tstreaminfo.h"

namespace Trd {
    
    class TrPkg;


/**
 * @brief TRPX Table of content section containing offsets to another sections 
 * 
 */
class TrSectionDescriptor {

public:

    
    bool isValid();
    //because right after writing header we dont have access to other sections assuming
    //we are writing sections sequentionally
    std::expected<void, Err::TrdError> blankDescriptor();
    std::expected<void, Err::TrdError> write();
    std::expected<void, Err::TrdError> read();
    static std::expected<void, Err::TrdError> isSdPresent();

    static std::expected<u64, Err::TrdError> getStartOffset();
    static std::expected<u64, Err::TrdError> getEndOffset();

    const Impl::TRD_SECTION_DESCRIPTOR& getSD() const;
    

    static std::expected<Impl::TRD_SECTION_DESCRIPTOR, Err::TrdError> readBack(Impl::TStreamInfo& tStream);

    explicit TrSectionDescriptor(TrPkg& pkg) : trpkg(pkg) {};

private:
    friend class TrPkg;
    TrPkg& trpkg;
    static std::expected<void, Trd::Err::TrdError> ICheckCRC(const Impl::TRD_SECTION_DESCRIPTOR& sd);
    static std::expected<void, Trd::Err::TrdError> IValidateSD(const Impl::TRD_SECTION_DESCRIPTOR& sd);
    static std::expected<void, Trd::Err::TrdError> IFieldCheckSD(const Impl::TRD_SECTION_DESCRIPTOR& sd);


};

};