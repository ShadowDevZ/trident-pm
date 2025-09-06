//internal, not to be exposed to the end user/dev

#include "pkgio.h"
#include "trderr.h"

class TRDSdToken : LibTrident::PkgIO::Descriptor {
private:
    std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fstrInfo;
    bool WriteTUIDAt(std::fstream& stream, std::streampos loc);

public:
    LibTrident::LTSTATUS::TridentError e;
    TRDSdToken(std::shared_ptr<LibTrident::FstreamInfo::TrdFstreamInfo> fStreamInfo) :
    fstrInfo(fStreamInfo) {}
    TRDSdToken(const TRDSdToken& other) : fstrInfo(other.fstrInfo) {}
    TRDSdToken(TRDSdToken&& other) : fstrInfo(std::move(other.fstrInfo)) {}

    static foffset_t GetRawSD();
    static foffset_t GetRawSDEnd(); 
    
    bool WriteDescriptorTUID(bool beg) override;
    bool ReadDescriptorTUID(bool beg) override;
    u32  GenerateCRC() override;
};