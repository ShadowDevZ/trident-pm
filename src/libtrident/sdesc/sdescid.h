//internal, not to be exposed to the end user/dev
#pragma once
#include "pkgio.h"
#include "trderr.h"
#include "trdconsts.h"


class TRDSdToken : LibTrident::PkgIO::Descriptor {
private:
    std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr;
public:
    LibTrident::Err::TridentError e;
    TRDSdToken(std::shared_ptr<LibTrident::Tstream::TStreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDSdToken(const TRDSdToken& other) : wFstr(other.wFstr) {}
    TRDSdToken(TRDSdToken&& other) : wFstr(std::move(other.wFstr)) {}

    static constexpr foffset_t GetOptRawSDStart() {
        return LibTrident::Consts::HeaderConsts::LT_HDR_SZB_01A;
    }
    
    static constexpr foffset_t GetRawSDEnd() {
        return GetOptRawSDStart() + LibTrident::Consts::SD::TRD_SECTIONSD_SIZE;
    }
    
    bool WriteDescriptorSUID() override;
    bool ReadDescriptorSUID() override;
    u32  GenerateCRC() override;
};