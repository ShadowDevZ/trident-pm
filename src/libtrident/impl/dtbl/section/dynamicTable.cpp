#include "dynamicTable.h"
#include "libtrident.h"
#include "trderr.h"
#include "sdesc.h"
using namespace Trd;
using namespace Trd::Impl;
using eCode = Err::Code;
//mind you this is not static because now its only stupidly byte next after
//but in theory this section could be placed anywhere in the file
std::expected<u64, Trd::Err::TrdError> DtblDirectory::getOffset() {
    EXP_TRY(internalSD.read(false));
    auto off = internalSD.getEndOffset();
    if (!off.has_value())
        return off.error();

    return off.value() + 1;
}
std::expected<void, Trd::Err::TrdError> DtblDirectory::writeSDEntry(u64 offset, u64 size,
                                                                    bool available) {

    if (available && offset < Consts::Header::LT_HDR_SZB_01A + Consts::SD::TRD_SECTIONSD_SIZE)
        return std::unexpected(
            Err::TrdError(eCode::SectionSizeViolated, 1,
                          "Tried to perform write operation in place of header or offset"));
    if (available && size == 0)
        return std::unexpected(Err::TrdError(eCode::SectionSizeViolated, 1,
                                             "Tried to write valid SD entry with size 0"));

    EXP_TRY(internalSD.read(false));
    SD_TBLENTRY dtblEntry{available};
    dtblEntry.offset = offset;
    dtblEntry.size = size;
    TRD_SD_UPDATEFIELD suf;
    suf.tblDynamic = dtblEntry;

    EXP_TRY(internalSD.updateSD(suf, std::nullopt, false));

    return {};
}

std::expected<void, Trd::Err::TrdError> DtblDirectory::invalidateSDEntry() {
    return writeSDEntry(0, 0, false);
}
std::expected<SD_TBLENTRY, Trd::Err::TrdError> DtblDirectory::readSDEntry() {
    EXP_TRY(internalSD.read(false));
    return internalSD.getSD().tblDynamic;
}
std::expected<void, Trd::Err::TrdError> DtblDirectory::writeRawEntry(std::span<const u8> data,
                                                                     u64 writeOffset) {
    if (data.empty())
        return std::unexpected(
            Err::TrdError(eCode::InvalidFuncArg, 1, "Empty data array was passed"));
    if (writeOffset < Consts::Header::LT_HDR_SZB_01A + Consts::SD::TRD_SECTIONSD_SIZE)
        return std::unexpected(
            Err::TrdError(eCode::SectionSizeViolated, 1,
                          "Tried to perform write operation in place of header or offset"));
    //sd needs to be present alongside the header
    EXP_TRY(readSDEntry());
    BinarySerializer::writeDataToTStream(trpkg.fstrInfo, data, false, writeOffset);
    return {};
}