#pragma once
#if defined(__linux__) || defined(__unix__)


#include <optional>
#include <filesystem>
#include <unistd.h>
namespace LinuxSpecific {
    std::expected<std::filesystem::path, LibTrident::Err::TrdError> CreateTemporaryFile() {
        char templatePath[] = "/tmp/.tmp_tridentpkgXXXXXX";
        int fd = mkstemp(templatePath);
        if (fd == -1) {
            return std::unexpected(LibTrident::Err::TrdError(LibTrident::Err::Code::FileWriteFailure));
        }
        return std::filesystem::path(templatePath);
    }
};

#endif