//internal, not to be exposed to the end user/dev

#include "pkgio.h"
#include "trderr.h"

class TRDSdToken : LibTrident::PkgIO::Descriptor {
private:
    std::weak_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> wFstr;
    bool WriteTUIDAt(std::streampos loc);

public:
    LibTrident::LTSTATUS::TridentError e;
    TRDSdToken(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) :
    wFstr(fStreamInfo) {}
    TRDSdToken(const TRDSdToken& other) : wFstr(other.wFstr) {}
    TRDSdToken(TRDSdToken&& other) : wFstr(std::move(other.wFstr)) {}

    static foffset_t GetRawSD();
    static foffset_t GetRawSDEnd(); 
    
    bool WriteDescriptorTUID(bool beg) override;
    bool ReadDescriptorTUID(bool beg) override;
    u32  GenerateCRC() override;
};