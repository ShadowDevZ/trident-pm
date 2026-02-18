//temporarily disabled for testin
/*

#include "sdescid.h"
#include "trheader.h"
#include "tstreaminfo.h"
#include "suid.h"
#include "binarySerializer.h"


using namespace Trd::Tstream;
using namespace Trd::UID;
using namespace Trd;
using eCode = Err::Code;

//address right after header

constexpr foffset_t GetSDAddress() { 
    //todo actually find the TUID inside the stream and get its position to check presence start
    return TRDSdToken::GetOptRawSDStart() + Trd::Consts::SUID::SUID_MAX_LENGTH + 1;
}
constexpr foffset_t GetSDEnd() {
    return GetSDAddress() + TRDSdToken::GetRawSDEnd() + Trd::Consts::SUID::SUID_MAX_LENGTH + 1;
}


//todo
//we need more error checking to check if the header is actually written



std::expected<void, Err::TrdError> TRDSdToken::WriteDescriptorSUID() {
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        return std::unexpected(Err::TrdError(eCode::ReferenceExpired));
    }
    auto sdStream = haveCtx.value();
    if (!sdStream->CheckFileStreamInfo()) {
        return std::unexpected(Err::TrdError(eCode::NullObject));
    }
    std::streampos sdOffset= static_cast<std::streampos>(GetOptRawSDStart());
    
 
    return SUID::WriteSUIDAt(sdStream, sdOffset, SUID::SUIDS::SECTION_DESCR);
    
}
std::expected<void, Err::TrdError> TRDSdToken::ReadDescriptorSUID() {
    return std::unexpected(Err::TrdError(eCode::FunctionNotImplemented));
}
bool TRDSdToken::IsValidSUID() {
    return false;
}

*/