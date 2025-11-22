#pragma once
#if defined(__linux__) || defined(__unix__)


#include <optional>
#include <filesystem>
#include <unistd.h>
namespace LinuxSpecific {
    std::optional<std::filesystem::path> CreateTemporaryFile() {
        char templatePath[] = "/tmp/.tmp_tridentpkgXXXXXX";
        int fd = mkstemp(templatePath);
        if (fd == -1) {
            return std::nullopt;
        }
        return std::filesystem::path(templatePath);
    }
};

#endif