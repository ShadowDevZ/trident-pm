//internal, not to be exposed to the end user/dev
#pragma once
#include "binarySerializer.h"
#include "trderr.h"
#include "trdconsts.h"
#include "datatypes.h"
namespace Trd::Impl {

class TRDSdToken : Trd::Impl::Descriptor {
private:
    std::weak_ptr<Trd::Impl::TStreamInfo> wFstr;
public:
    
    TRDSdToken(std::shared_ptr<Trd::Impl::TStreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDSdToken(const TRDSdToken& other) : wFstr(other.wFstr) {}
    TRDSdToken(TRDSdToken&& other) : wFstr(std::move(other.wFstr)) {}

    static constexpr Trd::foffset_t GetOptRawSDStart() {
        return Trd::Consts::Header::LT_HDR_SZB_01A;
    }
    
    static constexpr Trd::foffset_t GetRawSDEnd() {
        return GetOptRawSDStart() + Trd::Consts::SD::TRD_SECTIONSD_SIZE;
    }
    
    std::expected<void, Trd::Err::TrdError> WriteDescriptorSUID() override;
    std::expected<void, Trd::Err::TrdError> ReadDescriptorSUID() override;
    bool IsValidSUID() override;
};

};