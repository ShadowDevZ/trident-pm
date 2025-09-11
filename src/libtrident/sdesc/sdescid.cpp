#include "sdescid.h"
#include "trheader.h"
#include "fstreaminfo.h"
#include "suid.h"
#include "pkgio.h"

using namespace LibTrident::Header;
using namespace LibTrident::FstreamInfo;
using namespace LibTrident::UID;
using namespace LibTrident;
#define TRD_SECTIONSD_SIZE 32

//address right after header
foffset_t TRDSdToken::GetRawSD() {
    return TRDPkgHeader::GetHeaderByteSize();
}
foffset_t TRDSdToken::GetRawSDEnd() {
    return GetRawSD() + TRD_SECTIONSD_SIZE + 1 + SUID::SUID_MAX_LENGTH;
}
//todo
//we need more error checking to check if the header is actually written

bool TRDSdToken::WriteHeaderSUIDAt(std::streampos loc) {  
   
    auto [checkWeakRef, sharedPtr] = FstreamInfo::TrdFstreamInfo::GetFstreamContent(wFstr);
    if (!checkWeakRef) {
        e.SetError(LTSTATUS::IREF_EXPIRED);
        return false;
    }
    if (!sharedPtr->CheckFileStreamInfo()) {
        e.SetError(LTSTATUS::NULL_OBJ);
        return false;
    }
    LTSTATUS::LTSTATUS errCodePresent = TRDPkgHeader::IsHeaderPresent(sharedPtr);
    if (errCodePresent != LTSTATUS::SUCCESS) {
        e.SetError(errCodePresent);
        return false;
    }
    auto& fstrInfo = sharedPtr->GetFstreamObject();
    auto& fstrStream = fstrInfo.hFile;
    
    fstrStream->seekp(loc);

    if (!fstrStream) {
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    const char* SUID = SUID::GetSUIDString(SUID::SUID_SECDESC);
    if (!SUID::IsValidSUID(SUID)) {
        e.SetError(LTSTATUS::INVSUID);
        return false;
    } 

    //stream.WriteHeader(static_cast<const char*>(SUID), SUID::SUID_MAX_LENGTH);
    bool WriteHeaderStatus = PkgIO::FileOperations::WriteHeaderLeStream(fstrInfo, static_cast<const char*>(SUID), SUID::SUID_MAX_LENGTH, true);
    if (!fstrStream|| !WriteHeaderStatus) {
        e.SetError(LTSTATUS::IOWriteHeader);
        return false;
    }
    e.Success();
    return true;
}

bool TRDSdToken::WriteHeaderDescriptorSUID(bool beg) {
    if (beg) {
        std::streampos sdStart = static_cast<std::streampos>(GetRawSD());
        return WriteHeaderSUIDAt(sdStart);
    }
    else {
        std::streampos sdEnd = static_cast<std::streampos>(GetRawSDEnd());
        return WriteHeaderSUIDAt(sdEnd);
    }
    
}
bool TRDSdToken::ReadHeaderDescriptorSUID(bool beg) {
    (void)(beg);
    return false;
}
u32 TRDSdToken::GenerateCRC() {
    return 0;
}