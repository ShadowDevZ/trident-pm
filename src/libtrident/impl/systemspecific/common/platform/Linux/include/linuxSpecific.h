#pragma once
#if defined(__linux__) || defined(__unix__)


#include <expected>
#include <filesystem>
#include "trderr.h"
#include <sys/stat.h>
#include "portableTypes.h"
namespace LinuxSpecific {
    
    std::expected<std::filesystem::path, Trd::Err::TrdError> createTemporaryFile();
    std::expected<Trd::Impl::AuxiliaryStat, Trd::Err::TrdError> getAuxiliaryStat(const std::filesystem::path& path);
}
#endif