#include "sdesc.h"
#include "sdescid.h"
#include "trheader.h"
#include "pkgio.h"
#include "suid.h"
#include "trdconsts.h"
#include "fstreaminfo.h"
#include "memory.h"
using namespace LibTrident::SectionDescriptor;
using namespace LibTrident::Header;
using namespace LibTrident::UID;
//starting location of SD table without BEG/END token

//difference between these and raw functions is that Raw function point to the start of GUID whilst these point to actual data
//less error checking
foffset_t TRDSecDesc::GetSDAddress() { 
    //todo actually find the TUID inside the stream and get its position to check presence start
   return TRDSdToken::GetOptRawSDStart() + LibTrident::Consts::SUID::SUID_MAX_LENGTH;
}
foffset_t TRDSecDesc::GetSDEnd() {
    return GetSDAddress() + LibTrident::Consts::SD::TRD_SECTIONSD_SIZE;
}
//todo for normal write lookup the SUID using bmh algo from uid.cpp in future
bool TRDSecDesc::WriteBlankSD() {
    auto [checkWeakRef, sharedPtr] = FstreamInfo::TrdFstreamInfo::GetFstreamContent(wFstr);
    if (!checkWeakRef) {
        e.SetError(LTSTATUS::IREF_EXPIRED);
        return false;
    }
    if (!sharedPtr->CheckFileStreamInfo()) {
        e.SetError(LTSTATUS::NULL_OBJ);
        return false;
    }
    
    LTSTATUS::LTSTATUS hdrStatus = TRDPkgHeader::IsHeaderPresent(sharedPtr);
    if (hdrStatus != LTSTATUS::SUCCESS) {
        e.SetError(hdrStatus);
        return false;
    }
    auto& fstrInfo = sharedPtr->GetFstreamObject();
    auto& fstrStream = fstrInfo.hFile;
    
    TRDSdToken sdToken(sharedPtr);
    bool begSuidOk = sdToken.WriteDescriptorSUID();
    e.SetError(sdToken.e.GetError());
    if (!begSuidOk) {
        return false;
    }
    //reset in case it contains junk from previous operations
    memset(&secDescInternal, 0, sizeof(secDescInternal));

    const char* hdrContent = reinterpret_cast<const char*>(&secDescInternal);
    bool leStatus = PkgIO::FileOperations::WriteLeStream(fstrInfo, hdrContent, LibTrident::Consts::SD::TRD_SECTIONSD_SIZE, false);
    if (!leStatus || !fstrStream) {
        e.SetError(LTSTATUS::IO_WRITE);
        return false;
    }


    e.Success();
    return true;
}