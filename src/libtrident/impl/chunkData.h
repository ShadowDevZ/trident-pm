#pragma once
#include "datatypes.h"
#include "trdconsts.h"
#include <optional>
#include "tstreamDefs.h"
namespace Trd::Impl {
    /**
     * @brief for performing manual chunked IO operations 
     * 
     */
    struct IOReadChunk {
        std::array<u8, Consts::Binary::IO_CHUNK_SIZE> data;
        u32 bytesRead; // may be smaller then IO_CHUNK_SIZE
    };
    class IOChunkData {
      private:
        TRDFstreamObject xfInfo{};

      public:
        explicit IOChunkData(const TRDFstreamObject& obj) : xfInfo(obj) {};

        bool writeNextChunk(const char*& current, u64& remaining);
        static constexpr u32 chunkSize() {
            return Consts::Binary::IO_CHUNK_SIZE;
        }
        static u32 calculateChunkCount(u64 size) {
            return (size + Consts::Binary::IO_CHUNK_SIZE - 1) / Consts::Binary::IO_CHUNK_SIZE;
        }
        std::optional<IOReadChunk> readNextChunk(u64& remaining);
    };
};