#include "tstreaminfo.h"
// #include "fileOperations.h"
#include "ioflags.h"
#include "binarySerializer.h"
#include <cstring>
#include <ranges>
#include "chunkData.h"
using namespace Trd;
using namespace Trd::Impl;

std::expected<void, Err::TrdError> TStreamInfo::checkFileStreamInfo() const {
    // check if pointer was allocated usiong OpenPkg()

    if (!TStreamInfo::isOpen() || !xfInfo.hFile || !xfInfo.hFile->is_open()) {
        return std::unexpected(Err::TrdError{Err::Code::FileOpenFailure});
    }
    return {};
}

std::expected<void, Err::TrdError> TStreamInfo::isOpen() const {
    const Err::Code status = streamRemoteIsOpen(xfInfo);
    if (status != Err::Code::Success) {
        return std::unexpected(Err::TrdError{status});
    }
    return {};
}

std::expected<void, Err::TrdError> TStreamInfo::closeStream() {
    xfInfo.acccessModel._internal = _TrdInternalIO::IoClosed;
    return {};
}

void TStreamInfo::setFileStreamInfo(const TRDFstreamObject& info) {
    if (streamRemoteIsOpen(info) != Err::Code::Success) {
        throw std::runtime_error("Stream remote is closed");
    }

    xfInfo = info;
}

// does not check for endianness, the data should already be passed in as LE object
void TStreamInfo::writeTStream(const char* data, u64 size) {
    if (!checkFileStreamInfo() || !data || size < 1) {
        throw std::runtime_error("WriteLeStream(validate) Failed");
    }

    if (size == 0) {
        throw std::invalid_argument("Size was 0");
    }
    /*
#if LT_IO_ALWAYS_CHUNK == 1
    // xfInfo.hFile->write(data, size);
    IOChunkData cw{xfInfo};
    u64 remaining = size;
    const char* cursorData = data;

#ifdef _LIBTRIDENT_DEBUG_VERBOSE
    bool next = false;
    u32 noChunks = IOChunkData::calculateChunkCount(size);
    u32 chunksDone = 0;
    do {
        next = cw.writeNextChunk(cursorData, remaining);
        chunksDone++;
        dbgprintf("--processing chunk %u/%u\n\n\n", chunksDone, noChunks);
    } while (next);
#else
    while (cw.writeNextChunk(cursorData, remaining)) {};
#endif
*/
    /*
    IOChunkData chunkData{*this};
    chunkData.setupWrite(data, seekPos);
    bool next = false;
    do {
        next = chunkData.writeNextChunk2();
        auto debug = chunkData.getWriteData().value();
        dbgprintf("--cwrite2 %u/%u\n", debug.chunksDone, debug.noChunks);
    } while (next);
*/
    xfInfo.hFile->write(data, size);

    if (!xfInfo.hFile) {
        throw std::ios_base::failure("WriteLeData() Failed");
    }
}
// does not check for endianness, the data is retrieved as native endianness
//readTStream can never be read into chunk as this requires singular object to be returned
//if you need to read alrge amounts of data consider manually using readNextChunk
/* for example
 IOChunkData cw{xfInfo};
    u64 remaining = size;


    bool next = false;
    u32 noChunks = IOChunkData::calculateChunkCount(size);
    u32 chunksDone = 0;
    do {
        auto chunk = cw.readNextChunk(remaining);
        if (!chunk)
            break;
        next = remaining > 0;
        chunksDone++;
        dbgprintf("--read processing chunk %u/%u(%uB)\n\n\n", chunksDone, noChunks,
                  chunk.value().bytesRead);

        memcpy(s, chunk.value().data.data(), chunk.value().bytesRead);
    } while (next);

*/
void TStreamInfo::readTStream(char* s, u64 size) const {
    if (size == 0) {
        throw std::invalid_argument("Size was 0");
    }
    if (!checkFileStreamInfo() || !s || size < 1) {
        throw std::runtime_error("ReadLeStream(validate) Failed");
    }

    xfInfo.hFile->read(s, size);

    if (!xfInfo.hFile) {
        throw std::ios_base::failure("ReadLeData() Failed");
    }
}

void TStreamInfo::setSeekPos(u64 pos, std::ios_base::seekdir seekd) {
    xfInfo.hFile->seekg(pos, seekd);
    xfInfo.hFile->seekp(pos, seekd);
    if (!xfInfo.hFile) {

        // i was actually thinking if exceptions are necessary here but given that the user could set invalid offset
        // and this could invalidate the whole program means we would have to check seekpos in every function
        throw std::ios_base::failure("seekg() failure");
    }
}
i64 TStreamInfo::getSeekPos() const {
    const i64 pos = xfInfo.hFile->tellg();

    if (pos == -1 || !xfInfo.hFile) {

        throw std::ios_base::failure("tellg() failed");
    }
    return pos;
}
void TStreamInfo::flushData() {
    xfInfo.hFile->flush();
    if (!xfInfo.hFile)
        throw std::runtime_error("failed to flush fstream object");
}

void TStreamInfo::writePadding(u16 size, int value) {
    // char padding[size];
    std::vector<char> padding(size);
    std::ranges::fill(padding, value);
    // std::memset(padding, value, sizeof(padding));
    writeTStream(reinterpret_cast<const char*>(padding.data()), padding.size());
}
