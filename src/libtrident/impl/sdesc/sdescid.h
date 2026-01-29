//internal, not to be exposed to the end user/dev
#pragma once
#include "binarySerializer.h"
#include "trderr.h"
#include "trdconsts.h"
#include "datatypes.h"
namespace LibTrident::Impl {

class TRDSdToken : LibTrident::Impl::Descriptor {
private:
    std::weak_ptr<LibTrident::Impl::TStreamInfo> wFstr;
public:
    
    TRDSdToken(std::shared_ptr<LibTrident::Impl::TStreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDSdToken(const TRDSdToken& other) : wFstr(other.wFstr) {}
    TRDSdToken(TRDSdToken&& other) : wFstr(std::move(other.wFstr)) {}

    static constexpr LibTrident::foffset_t GetOptRawSDStart() {
        return LibTrident::Consts::Header::LT_HDR_SZB_01A;
    }
    
    static constexpr LibTrident::foffset_t GetRawSDEnd() {
        return GetOptRawSDStart() + LibTrident::Consts::SD::TRD_SECTIONSD_SIZE;
    }
    
    std::expected<void, LibTrident::Err::TrdError> WriteDescriptorSUID() override;
    std::expected<void, LibTrident::Err::TrdError> ReadDescriptorSUID() override;
    bool IsValidSUID() override;
};

};