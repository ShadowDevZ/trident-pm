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
        std::vector<std::byte> data;

        // u32 bytesRead; // may be smaller then IO_CHUNK_SIZE
    };
    struct IOWriteData {
        std::span<const std::byte> remaining{};
        u32 noChunks{0};
        u32 chunksDone{0};
        u64 _writeSeek{0};
        bool isInit{false};
    };
    struct IOReadData {
        u64 remaining{0};
        u32 noChunks{0};
        u32 chunksDone{0};
        u64 _readSeek{0};
        bool isInit{false};
    };

    class IOChunkData {
      private:
        //TRDFstreamObject xfInfo{};
        IOWriteData writeData{};
        IOReadData readData{};
        Trd::Impl::TStreamInfo& xfInfo;
        Trd::Impl::TRDFstreamObject& fstrObj;

      public:
        explicit IOChunkData(Trd::Impl::TStreamInfo& obj) :
            xfInfo(obj), fstrObj(obj.getFstreamObject()) {};
        /**
         * @brief returns writedata if its initialized
         * 
         * @return std::optional<IOWriteData> 
         */
        std::optional<IOWriteData> getWriteData() const {
            if (writeData.isInit)
                return writeData;
            return std::nullopt;
        }
        /**
         * @brief returns readdata if its initialized
         * 
         * @return std::optional<IOReadData> 
         */
        std::optional<IOReadData> getReadData() const {
            if (readData.isInit)
                return readData;
            return std::nullopt;
        }

        // bool writeNextChunk(const char*& current, u64& remaining);
        // std::optional<IOReadChunk> readNextChunk(u64& remaining);

        bool writeNextChunk();
        std::optional<IOReadChunk> readNextChunk();

        void setupWrite(std::span<const std::byte> data, file_offset offsetWrite);
        void setupRead(file_offset offsetRead, u64 size);
        static constexpr u32 chunkSize() {
            return Consts::Binary::IO_CHUNK_SIZE;
        }
        static u32 calculateChunkCount(u64 size) {
            return (size + Consts::Binary::IO_CHUNK_SIZE - 1) / Consts::Binary::IO_CHUNK_SIZE;
        }
    };
};