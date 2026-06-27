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

    if (available && offset < badOffset())
        return std::unexpected(
            Err::TrdError(eCode::SectionSizeViolated, 1,
                          "Tried to perform write operation in place of header or offset"));
    if (available && size == 0)
        return std::unexpected(Err::TrdError(eCode::SectionSizeViolated, 1,
                                             "Tried to write valid SD entry with size 0"));

    EXP_TRY(internalSD.read(false));
    SD_TBLENTRY dtblEntry;
    dtblEntry.offset = offset;
    dtblEntry.size = size;
    dtblEntry.available = u8bool::fromBool(available);
    TRD_SD_UPDATEFIELD suf;
    suf.tblDynamic = dtblEntry;

    EXP_TRY(internalSD.updateSD(suf, std::nullopt, false));

    return {};
}

std::expected<void, Trd::Err::TrdError> DtblDirectory::invalidateSDEntry() {
    return writeSDEntry(0, 0, false);
}
std::expected<SD_TBLENTRY, Trd::Err::TrdError> DtblDirectory::readSDEntry() const {
    EXP_TRY(internalSD.read(false));
    return internalSD.getSD().tblDynamic;
}
std::expected<void, Trd::Err::TrdError>
DtblDirectory::writeRawRegion(std::span<const std::byte> data, u64 writeOffset) const {
    if (data.empty())
        return std::unexpected(
            Err::TrdError(eCode::InvalidFuncArg, 1, "Empty data array was passed"));
    if (writeOffset < badOffset())
        return std::unexpected(
            Err::TrdError(eCode::SectionSizeViolated, 1,
                          "Tried to perform write operation in place of header or offset"));
    //sd needs to be present alongside the header
    EXP_TRY(trpkg.fstrInfo.checkFileStreamInfo());
    EXP_TRY(readSDEntry());
    BinarySerializer::writeDataToTStream(trpkg.fstrInfo, data, false, writeOffset);
    return {};
}
std::expected<std::vector<std::byte>, Trd::Err::TrdError>
DtblDirectory::readRawRegion(u64 readOffset, u64 size) const {
    std::vector<std::byte> readData(size);
    EXP_TRY(trpkg.fstrInfo.checkFileStreamInfo());

    if (readOffset < badOffset())
        return std::unexpected(
            Err::TrdError(eCode::SectionSizeViolated, 1,
                          "Tried to perform read operation in place of header or offset"));
    readData = BinarySerializer::readDataFromTStream(trpkg.fstrInfo, readOffset, size, false);
    return readData;
}
std::expected<void, Trd::Err::TrdError>
DtblDirectory::writeRegionInChunks(std::span<const std::byte> data, u64 writeOffset) const {
    //todo call tregHaveValidEntry(writeOffset)
    if (data.empty())
        return std::unexpected(Err::TrdError(eCode::NullObject, 1, "data was empty"));
    if (writeOffset < badOffset())
        return std::unexpected(
            Err::TrdError(eCode::SectionSizeViolated, 1,
                          "Tried to perform read operation in place of header or offset"));

    IOChunkData chunkData{trpkg.getTstream()};
    chunkData.setupWrite(data, writeOffset);
    bool next = false;
    do {
        next = chunkData.writeNextChunk();

#ifdef _LIBTRIDENT_DEBUG_VERBOSE
        auto debug = chunkData.getWriteData();
        [[unlikely]]
        if (!debug)
            throw std::runtime_error("writedata ctx was empty/internal error");
        dbgprintf("__cwrite2 %u/%u\n", debug.value().chunksDone, debug.value().noChunks);
#endif
    } while (next);
    return {};
}
DtblDirectory::DtblDirectory(TrPkg& pkg) :
    trpkg{pkg}, internalSD{pkg}, readChunkBuffer{pkg.fstrInfo} {};

std::expected<void, Trd::Err::TrdError> DtblDirectory::readRegionChunkSetup(u64 setupRead,
                                                                            u64 size) {
    if (setupRead < badOffset())
        return std::unexpected(
            Err::TrdError(eCode::SectionSizeViolated, 1,
                          "Tried to perform read operation in place of header or offset"));
    EXP_TRY(trpkg.fstrInfo.checkFileStreamInfo());
    readChunkBuffer.setupRead(setupRead, size);
    return {};
}

std::expected<Trd::Impl::IOReadChunk, Trd::Err::TrdError> DtblDirectory::readNextRegionChunk() {

    auto r = readChunkBuffer.readNextChunk();
    if (!r)
        return std::unexpected(Err::TrdError(eCode::FileReadFailure, 1, "chunk read fail"));
    const auto& readChunk = r.value();
    if (readChunk.data.empty())
        return std::unexpected(
            Err::TrdError(eCode::FileReadFailure, 1, "read 0 bytes inside chunk"));

    return readChunk;
    //todo call tregHaveValidEntry(writeOffset)
}

std::expected<void, Trd::Err::TrdError> DtblDirectory::invalidateRegion(u64 writeOffset,
                                                                        u64 len) const {
    std::vector<std::byte> data(len, std::byte{0xFF});
    return writeRawRegion(data, writeOffset);
};