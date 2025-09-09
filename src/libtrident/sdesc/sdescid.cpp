#include "sdescid.h"
#include "trheader.h"
#include "fstreaminfo.h"
#include "tuid.h"
#include "pkgio.h"
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

bool TRDSdToken::WriteTUIDAt(std::streampos loc) {  

    auto [checkWeakRef, sharedPtr] = FstreamInfo::TrdFstreamInfo::GetFstreamContent(wFstr);
    if (!checkWeakRef) {
        e.SetError(LTSTATUS::IREF_EXPIRED);
        return false;
    }
    if (!sharedPtr->CheckFileStreamInfo()) {
        e.SetError(LTSTATUS::NULL_OBJ);
        return false;
    }
    auto& fstrInfo = sharedPtr->GetFileStreamInfo();
    auto& fstrStream = fstrInfo.hFile;
    
    fstrStream->seekp(loc);

    if (!fstrStream) {
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    const char* tuid = TUID::GetTUIDString(TUID::TUID_SECDESC);
    if (!TUID::IsValidTUID(tuid)) {
        e.SetError(LTSTATUS::INVTUID);
        return false;
    } 

    //stream.write(static_cast<const char*>(tuid), TUID::TUID_MAX_LENGTH);
    bool writeStatus = PkgIO::FileOperations::WriteLeStream(fstrInfo, static_cast<const char*>(tuid), TUID::TUID_MAX_LENGTH, true);
    if (!fstrStream|| !writeStatus) {
        e.SetError(LTSTATUS::IOWRITE);
        return false;
    }
    e.Success();
    return true;
}

bool TRDSdToken::WriteDescriptorTUID(bool beg) {
    if (beg) {
        std::streampos sdStart = static_cast<std::streampos>(GetRawSD());
        return WriteTUIDAt(sdStart);
    }
    else {
        std::streampos sdEnd = static_cast<std::streampos>(GetRawSDEnd());
        return WriteTUIDAt(sdEnd);
    }
    
}
bool TRDSdToken::ReadDescriptorTUID(bool beg) {
    (void)(beg);
    return false;
}
u32 TRDSdToken::GenerateCRC() {
    return 0;
}