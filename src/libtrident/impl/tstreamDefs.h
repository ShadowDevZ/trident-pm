#pragma once
#include <filesystem>
#include <memory>
#include "portableTypes.h"
#include "ioflags.h"
#include <fstream>
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
    static inline Err::Code streamRemoteIsOpen(const TRDFstreamObject& info) {
        if (info.acccessModel._internal == _TrdInternalIO::IoClosed) {
            return Err::Code::FileOpenFailure;
        }

        if (!info.hFile || !info.hFile->is_open() || (!info.fileOpened)) {

            return Err::Code::FileOpenFailure;
        }

        return Err::Code::Success;
    }
};