#pragma once
#include <expected>
#include "ccattribs.h"
#include <filesystem>
#include "trderr.h"
#include "portableTypes.h"
#include <filesystem>
namespace LibTrident::SystemSpecific {
    std::expected<std::filesystem::path, LibTrident::Err::TrdError> CreateTemporaryFile();
    std::expected<LibTrident::PortableTypes::AuxiliaryStat, LibTrident::Err::TrdError> GetAuxiliaryStat(const std::filesystem::path& path);
    std::expected<LibTrident::PortableTypes::PortableStat, LibTrident::Err::TrdError> StatObject(const std::filesystem::path& file);

    inline std::expected<LibTrident::PortableTypes::PortableStat, LibTrident::Err::TrdError> StatFile(const std::filesystem::path& file) {
        if (!std::filesystem::is_regular_file(file)) {
            return std::unexpected(Err::TrdError{Err::Code::ObjectNotFile});
        }
        return StatObject(file);
    }

};