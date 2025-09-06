#include "sdescid.h"
#include "trheader.h"
#include "fstreaminfo.h"
#include "tuid.h"
using namespace LibTrident::Header;
using namespace LibTrident::FstreamInfo;
using namespace LibTrident;

#define TRD_SECTIONSD_SIZE 32

//address right after header
foffset_t TRDSdToken::GetRawSD() {
    return TRDPkgHeader::GetHeaderByteSize();
}
foffset_t TRDSdToken::GetRawSDEnd() {
    return GetRawSD() + TRD_SECTIONSD_SIZE + 1 + TUID::TUID_MAX_LENGTH;
}
//todo
//we need more error checking to check if the header is actually written

bool TRDSdToken::WriteTUIDAt(std::fstream& stream, std::streampos loc) {  
    stream.seekp(loc);

    if (!stream) {
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    const char* tuid = TUID::GetTUIDString(TUID::TUID_SECDESC);
    if (!TUID::IsValidTUID(tuid)) {
        e.SetError(LTSTATUS::INVTUID);
        return false;
    } 
    stream.write(static_cast<const char*>(tuid), TUID::TUID_MAX_LENGTH);
    if (!stream) {
        e.SetError(LTSTATUS::IOWRITE);
        return false;
    }
    e.Success();
    return true;
}

bool TRDSdToken::WriteDescriptorTUID(bool beg) {
    std::fstream& stream = *fstrInfo->GetFileStreamInfo()->hFile;
    
    if (!fstrInfo->CheckFileStreamInfo()) {
        return false;
    }
    if (beg) {
        std::streampos sdStart = static_cast<std::streampos>(GetRawSD());
        return WriteTUIDAt(stream, sdStart);
    }
    else {
        std::streampos sdEnd = static_cast<std::streampos>(GetRawSDEnd());
        return WriteTUIDAt(stream, sdEnd);
    }
    
}
bool TRDSdToken::ReadDescriptorTUID(bool beg) {
    return false;
}
u32 TRDSdToken::GenerateCRC() {
    return 0;
}