#pragma once
#if defined(__linux__) || defined(__unix__)


#include <expected>
#include <filesystem>
#include "trderr.h"
#include <sys/stat.h>
#include "portableTypes.h"
namespace LinuxSpecific {
    
    std::expected<std::filesystem::path, LibTrident::Err::TrdError> CreateTemporaryFile();
    std::expected<LibTrident::PortableTypes::AuxiliaryStat, LibTrident::Err::TrdError> GetAuxiliaryStat(const std::filesystem::path& path);
}
#endif