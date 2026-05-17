#include "dynamicTable.h"
#include "libtrident.h"
#include "sdesc.h"
using namespace Trd;
using namespace Trd::Impl;

std::expected<u64, Trd::Err::TrdError> DtblDirectory::findFreeOffset() const {
    EXP_TRY(Trd::TrSectionDescriptor::isSdPresent(trpkg.fstrInfo));
    return 12;
}