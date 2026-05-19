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
