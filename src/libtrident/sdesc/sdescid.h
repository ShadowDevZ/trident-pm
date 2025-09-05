#include "pkgio.h"

class TRDSdToken : LibTrident::PkgIO::Descriptor {
private:
    std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fstrInfo;

public:
    TRDSdToken(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) :
    fstrInfo(fStreamInfo) {}
    TRDSdToken(const TRDSdToken& other) : fstrInfo(other.fstrInfo) {}
    TRDSdToken(TRDSdToken&& other) : fstrInfo(std::move(other.fstrInfo)) {}

    bool WriteDescriptorUUID() override;
    bool ReadDescriptorUUID() override;
    u32  GenerateCRC() override;
};