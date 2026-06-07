#include "datatypes.h"
#include <memory>
#include "tstreamDefs.h"
#include <fstream>
#include "chunkData.h"
using namespace Trd;
using namespace Trd::Impl;

bool IOChunkData::writeNextChunk(const char*& current, u64& remaining) {
    if (streamRemoteIsOpen(xfInfo) != Err::Code::Success)
        throw std::runtime_error("stream not open");
    if (remaining == 0)
        return false;
    u32 toWrite = static_cast<u32>(std::min<u64>(remaining, Consts::Binary::IO_CHUNK_SIZE));
    xfInfo.hFile->write(current, toWrite);
    if (!xfInfo.hFile)
        throw std::runtime_error("Chunk write failed fstream error");

    current += toWrite;
    remaining -= toWrite;
    return remaining > 0;
}
std::optional<IOReadChunk> IOChunkData::readNextChunk(u64& remaining) {
    if (streamRemoteIsOpen(xfInfo) != Err::Code::Success)
        throw std::runtime_error("stream not open");
    if (remaining == 0)
        return std::nullopt;
    u32 toRead = static_cast<u32>(std::min<u64>(remaining, Consts::Binary::IO_CHUNK_SIZE));
    IOReadChunk ioRead{};
    xfInfo.hFile->read(reinterpret_cast<char*>(ioRead.data.data()), toRead);
    u32 readBytes = static_cast<u32>(xfInfo.hFile->gcount());
    if (!xfInfo.hFile)
        throw std::runtime_error("chunk read fstream error");
    if (readBytes == 0)
        return std::nullopt; //EOF ?

    ioRead.bytesRead = readBytes;
    remaining -= readBytes;

    return ioRead;
}