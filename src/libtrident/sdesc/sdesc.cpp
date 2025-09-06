#include "sdesc.h"
#include "sdescid.h"
#include "trheader.h"
#include "pkgio.h"
#include "tuid.h"
using namespace LibTrident::SectionDescriptor;
using namespace LibTrident::Header;
//starting location of SD table without BEG/END token
foffset_t TRDSecDesc::IGetSDAddress() { 
    //todo actually find the TUID inside the stream and get its position to check presence start
    return TRDSdToken::GetRawSD() + TUID::TUID_MAX_LENGTH + 1;
}
