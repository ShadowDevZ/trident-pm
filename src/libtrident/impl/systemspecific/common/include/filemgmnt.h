#pragma once
#include <expected>
#include "ccattribs.h"
#include <filesystem>
#include "trderr.h"
#include "portableTypes.h"
#include <filesystem>
namespace LibTrident::Impl {
    std::expected<std::filesystem::path, LibTrident::Err::TrdError> createTemporaryFile();
    std::expected<AuxiliaryStat, LibTrident::Err::TrdError> getAuxiliaryStat(const std::filesystem::path& path);
    std::expected<PortableStat, LibTrident::Err::TrdError> statObject(const std::filesystem::path& file);

    inline std::expected<PortableStat, LibTrident::Err::TrdError> statFile(const std::filesystem::path& file) {
        if (!std::filesystem::is_regular_file(file)) {
            return std::unexpected(Err::TrdError{Err::Code::ObjectNotFile});
        }
        return statObject(file);
    }

};