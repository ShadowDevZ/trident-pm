#include "datatypes.h"
#include <memory>
#include "tstreamDefs.h"
#include <fstream>
#include "chunkData.h"
#include "binarySerializer.h"

using namespace Trd;
using namespace Trd::Impl;
void IOChunkData::setupWrite(std::span<const std::byte> data, u64 offsetWrite) {
    if (data.empty())
        throw std::runtime_error("empty (null) data passed");
    writeData.chunksDone = 0;
    writeData.remaining = data;
    writeData.noChunks = calculateChunkCount(data.size_bytes());
    writeData._writeSeek = offsetWrite;
    writeData.isInit = true;
}
void IOChunkData::setupRead(u64 offsetRead, u64 size) {
    if (size == 0)
        throw std::runtime_error("tried to read 0 bytes");
    readData.chunksDone = 0;
    readData.noChunks = calculateChunkCount(size);
    readData.remaining = size;
    readData._readSeek = offsetRead;
    readData.isInit = true;
}
bool IOChunkData::writeNextChunk() {
    if (!writeData.isInit)
        throw std::runtime_error("data has to be initialized first via setup function");
    if (writeData.remaining.empty())
        return false;
    auto chunk = writeData.remaining.first(
        std::min<u64>(writeData.remaining.size(), Consts::Binary::IO_CHUNK_SIZE));
    //this lead to accidental recursion

    BinarySerializer::writeDataToTStream(xfInfo, chunk, false, writeData._writeSeek);

    //xfInfo.writeTStream(reinterpret_cast<const char*>(chunk.data()), chunk.size_bytes());
    writeData._writeSeek += chunk.size_bytes();
    writeData.remaining = writeData.remaining.subspan(chunk.size());
    writeData.chunksDone++;

    return !writeData.remaining.empty();
}

std::optional<IOReadChunk> IOChunkData::readNextChunk() {
    if (!readData.isInit)
        throw std::runtime_error("data has to be initialized first via setup function");
    if (readData.remaining == 0)
        return std::nullopt;
    //could not flushing the data cause problems ? i honestly dont know but rather be safe than sorry
    //xfInfo.flushData();

    u32 toRead = static_cast<u32>(std::min<u64>(readData.remaining, Consts::Binary::IO_CHUNK_SIZE));
    IOReadChunk readChunk{};
    //auto dataVector =
    readChunk.data =
        BinarySerializer::readDataFromTStream(xfInfo, readData._readSeek, toRead, false);
    // std::ranges::move(dataVector, readChunk.data.begin());

    u32 readBytes = static_cast<u32>(fstrObj.hFile->gcount());
    if (!fstrObj.hFile)
        throw std::runtime_error("chunk read fstream error");
    if (readBytes == 0)
        return std::nullopt; //EOF?

    [[unlikely]]
    if (readChunk.data.size() > Consts::Binary::IO_CHUNK_SIZE)
        throw std::runtime_error("IO_CHUNK size was bigger than max allowed IO_CHUNK_SIZE");

    // readChunk.bytesRead = readBytes;
    readData._readSeek += readBytes;
    readData.remaining -= readBytes;
    readData.chunksDone++;

    return readChunk;
}
/*
bool IOChunkData::writeNextChunk(const char*& current, u64& remaining) {
    if (current == nullptr)
        throw std::runtime_error("tried to pass nullptr");
    if (streamRemoteIsOpen(fstrObj) != Err::Code::Success)
        throw std::runtime_error("stream not open");
    if (remaining == 0)
        return false;
    u32 toWrite = static_cast<u32>(std::min<u64>(remaining, Consts::Binary::IO_CHUNK_SIZE));

    fstrObj.hFile->write(current, toWrite);
    if (!fstrObj.hFile)
        throw std::runtime_error("Chunk write failed fstream error");

    current += toWrite;
    remaining -= toWrite;
    return remaining > 0;
}
std::optional<IOReadChunk> IOChunkData::readNextChunk(u64& remaining) {
    if (streamRemoteIsOpen(fstrObj) != Err::Code::Success)
        throw std::runtime_error("stream not open");
    if (remaining == 0)
        return std::nullopt;
    u32 toRead = static_cast<u32>(std::min<u64>(remaining, Consts::Binary::IO_CHUNK_SIZE));
    IOReadChunk ioRead{};
    fstrObj.hFile->read(reinterpret_cast<char*>(ioRead.data.data()), toRead);
    u32 readBytes = static_cast<u32>(fstrObj.hFile->gcount());
    if (!fstrObj.hFile)
        throw std::runtime_error("chunk read fstream error");
    if (readBytes == 0)
        return std::nullopt; //EOF ?

    ioRead.bytesRead = readBytes;
    remaining -= readBytes;

    return ioRead;
}
    */