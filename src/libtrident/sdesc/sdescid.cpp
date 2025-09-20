#include "sdescid.h"
#include "trheader.h"
#include "tstreaminfo.h"
#include "suid.h"
#include "pkgio.h"

using namespace LibTrident::Header;
using namespace LibTrident::TstreamInfo;
using namespace LibTrident::UID;
using namespace LibTrident;


//address right after header

foffset_t GetSDAddress() { 
    //todo actually find the TUID inside the stream and get its position to check presence start
    return TRDSdToken::GetOptRawSDStart() + LibTrident::Consts::SUID::SUID_MAX_LENGTH + 1;
}
foffset_t GetSDEnd() {
    return GetSDAddress() + TRDSdToken::GetRawSDEnd() + LibTrident::Consts::SUID::SUID_MAX_LENGTH + 1;
}


//todo
//we need more error checking to check if the header is actually written



bool TRDSdToken::WriteDescriptorSUID() {
    auto haveCtx = TstreamInfo::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        e.SetError(Err::Code::IREF_EXPIRED);
        return false;
    }
    auto sharedPtr = haveCtx.value();
    if (!sharedPtr->CheckFileStreamInfo()) {
        e.SetError(Err::Code::NULL_OBJ);
        return false;
    }
    std::streampos sdOffset= static_cast<std::streampos>(GetOptRawSDStart());
    
 
    Err::Code errSuid = SUID::WriteSUIDAt(sharedPtr, sdOffset, SUID::SUIDS::SECTION_DESCR);
    e.SetError(errSuid);
    if (errSuid == Err::Code::OK) {
        return true;
    }
    return false;
    
}
bool TRDSdToken::ReadDescriptorSUID() {
    return false;
}
u32 TRDSdToken::GenerateCRC() {
    return 0;
}