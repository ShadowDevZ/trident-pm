#include "binarySerializer.h"
#include <sys/stat.h>
#include <cstdio>
#include "ccattribs.h"
#include <cerrno>
#include <format>
#include "filemgmnt.h"

#include <cstring>
#include "chunkData.h"
using namespace Trd;
using namespace Trd::Impl;

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#error                                                                                             \
    "The implementation on Big Endian is currently completely broken. DO NOT USE THIS PROGRAM ON BIG ENDIAN SYSTEM"
#endif

void BinarySerializer::addRaw(const void* data, u64 size) {
    if (!data || size == 0) {
        throw std::invalid_argument("AddRaw() failed. Data or size is 0");
    }
    const std::byte* bytes = static_cast<const std::byte*>(data);
    bufferData.insert(bufferData.end(), bytes, bytes + size);
}

u64 BinarySerializer::readRaw(void* dataOut, u64 size, file_offset offset) {
    if (dataOut == nullptr) {
        throw std::invalid_argument("nullptr was passed");
    }
    if (offset + size > bufferData.size()) {
        const std::string pi =
            std::format("buffsz: {}, exp_atl: {}", bufferData.size(), offset + size);
        throw std::out_of_range("Buffer was not big enough " + pi);
    }

    if (size == 0) {
        throw std::runtime_error("Read 0 bytes");
    }
    u8* bytes = reinterpret_cast<u8*>(dataOut);
    std::memcpy(bytes, bufferData.data() + offset, size);

    return size;
}
//todo use std expected instead of exceptions
std::vector<std::byte> BinarySerializer::readDataFromTStream(Trd::Impl::TStreamInfo& tStream,
                                                             file_offset seekPos, u64 size,
                                                             bool requireAlignment,
                                                             bool keepOriginalSeek) {

    if (requireAlignment && !isDataSizeAligned(size)) {
        throw std::invalid_argument("Data size not aligned");
    }
    if (!tStream.checkFileStreamInfo()) {
        throw std::runtime_error("CheckFileStreamInfo() failed");
    }
    const i64 ogSeek = tStream.getSeekPos();
    tStream.setSeekPos(seekPos);
    std::vector<std::byte> data(size);
    // data.reserve(size);

    tStream.readTStream(reinterpret_cast<char*>(data.data()), size);
    if (keepOriginalSeek)
        tStream.setSeekPos(ogSeek);

    return data;
}

void BinarySerializer::writeDataToTStream(Trd::Impl::TStreamInfo& tStream,
                                          std::span<const std::byte> data, bool requireAlignment,
                                          file_offset seekPos, std::ios_base::seekdir seekDir,
                                          bool keepOriginalSeek) {
    //todo make this boilerplate in all classes a function

    if (data.empty()) {
        throw std::invalid_argument("Empty buffer was passed");
    }
    if (requireAlignment && !isDataSizeAligned(data.size())) {
        throw std::invalid_argument("Data size not aligned");
    }
    if (!tStream.checkFileStreamInfo()) {
        throw std::runtime_error("CheckFileStreamInfo() failed");
    }
    const file_offset ogSeek = tStream.getSeekPos();

    tStream.setSeekPos(seekPos, seekDir);
    /*IOChunkData chunkData{tStream};
    chunkData.setupWrite(data, seekPos);
    bool next = false;
    do {
        next = chunkData.writeNextChunk2();
        auto debug = chunkData.getWriteData().value();
        dbgprintf("--cwrite2 %u/%u\n", debug.chunksDone, debug.noChunks);
    } while (next);*/
    // tStream.writeTStream(reinterpret_cast<const char*>(data.data()), data.size_bytes());
    tStream.writeTStream(data);
    if (keepOriginalSeek)
        tStream.setSeekPos(ogSeek);
}

std::optional<std::vector<std::byte>> BinarySerializer::getFormattedData(bool autoAlign) {
    if (bufferData.empty()) {
        return std::nullopt;
    }

    u64 alignSize = 0;

    if (autoAlign) {
        const auto& vSize = bufferData.size();
        if (!isDataSizeAligned(vSize)) {
            alignSize = getByteAlignment(vSize) - vSize;
            dbgprintf("--Unaligned data serialized\nog:%luB new: %luB\n", vSize, alignSize + vSize);
        }
    }

    // bufferData.insert(bufferData.end(), alignSize, std::byte{0}); //???
#ifdef _LIBTRIDENT_DEBUG_VERBOSE
    dbgDumpData();
#endif
    if (!expectAlignedDataOrDie(bufferData.size())) {
        return std::nullopt;
    }
#if defined(_LIBTRIDENT_DEBUG_VERBOSE)
    dbgprintf("--Serializing data size %luB\n\n", bufferData.size());
#endif
    // bool st = tStream->writeTStream(reinterpret_cast<const char*>(bufferData.data()), bufferData.size(),  true);

    return bufferData;
}

u64 BinarySerializer::getByteAlignment(u64 varSize) {
    constexpr auto alignBytes = Consts::Binary::BSERIALIZE_DATA_ALIGN;

    if (varSize % alignBytes) {
        varSize += (alignBytes - (varSize % alignBytes));
    }

    return varSize;
}
bool BinarySerializer::isLittleEndian() noexcept {
#if LT_DEBUG_ENDIAN_FORCE == 1
    return true;
#elif LT_DEBUG_ENDIAN_FORCE == 2
    return false;
#else
    return std::endian::native == std::endian::little;
#endif
}
bool BinarySerializer::isInstanceLittleEndian() const noexcept {
#if LT_DEBUG_ENDIAN_FORCE == 1
    return true;
#elif LT_DEBUG_ENDIAN_FORCE == 2
    return false;
#else
    return emulEndianness == std::endian::little;
#endif
}
bool BinarySerializer::expectAlignedDataOrDie(u64 size) {
    //normal assert used because this condition simply cant happen
    const bool aligned = isDataSizeAligned(size);
    if (!aligned) {

        throw std::runtime_error("Passed data was not properly aligned got: " +
                                 std::to_string(size));
    }
    //just in case the assertion fails
    return aligned;
}
#ifdef _LIBTRIDENT_DEBUG
TRD_DBG_BUILD_ONLY void BinarySerializer::dbgDumpData() const {
    const auto& data = getData();

    dbgprintf("=====BS_DATA_DUMP(%lu,%s)======\n{", (data.size() * sizeof(std::byte)),
              isDataSizeAligned(data.size()) ? "aligned" : "!aligned");
    for (const auto& x : data) {
        dbgprintf(" 0x0%x ", static_cast<u8>(x));
    }
    dbgprintf("}\n===========\n");
}
#endif
//todo use ReadLeStream()
