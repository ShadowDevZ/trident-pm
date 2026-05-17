#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
#include <sys/stat.h>
#include <optional>
#include "ioflags.h"
#include <filesystem>
#include <expected>
#include "filemgmnt.h"
//#include "ioflags.h"
namespace Trd::Impl {

    struct TRDFstreamObject {
        //todo enforce PATHMAX and use const char* to avoid unnecessary memory allocation
        std::filesystem::path absolutePath;
        std::shared_ptr<std::fstream> hFile;
        //does not represent actual file size, but sizeof(whole file - header), not utilized yet
        // u64 checksumSize{0};
        TRDAccessModel acccessModel{};
        //for future use, atime
        Impl::PortableStat pStat{};
        //struct stat64 fileStat;
        bool fileOpened{false};
    };

    class TStreamInfo {
      private:
        TRDFstreamObject xfInfo{};

      public:
        TRDFstreamObject& getFstreamObject() {
            return xfInfo;
        }
        const TRDFstreamObject& getFstreamObject() const {
            return xfInfo;
        }
        //throws std::runtime on failure
        void setFileStreamInfo(const TRDFstreamObject& info);

        //checks if each field is correctly set
        std::expected<void, Err::TrdError> checkFileStreamInfo() const;
        std::expected<void, Err::TrdError> closeStream();
        //checks if the stream is only MARKED as open, data may be missing or corrupted
        std::expected<void, Err::TrdError> isOpen() const;
        //throws std::iosbase::failure on exception
        void setSeekPos(u64 pos, std::ios_base::seekdir seek = std::ios::beg);
        //throws std::iosbase::failure on exception
        i64 getSeekPos() const;

        template <typename T>
        void writeTStream(const T& t, u64 size) {
            writeTStream(reinterpret_cast<const char*>(&t), size);
        }
        template <typename T>
        void writeTStream(T& t) {
            writeTStream(t, sizeof(t));
        }

        //throws std::invalid_argument runtime error or anything by FileOperations::WriteLeData
        void writeTStream(const char* data, u64 size);
        void writePadding(u16 size, int value = 0);

        template <typename T>
        void readTStream(T& t, u64 size) const {
            readTStream(reinterpret_cast<char*>(&t), size);
        }

        template <typename T>
        void readTStream(T& t) const {
            readTStream(t, sizeof(t));
        }

        //throws std::invalid_argument runtime error or anything by FileOperations::ReadLeData
        void readTStream(char* s, u64 size) const;
    };

    //todo remove
};