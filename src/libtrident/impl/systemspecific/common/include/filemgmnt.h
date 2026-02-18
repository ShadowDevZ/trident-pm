#pragma once
#include <expected>
#include "ccattribs.h"
#include <filesystem>
#include "trderr.h"
#include "portableTypes.h"
#include <filesystem>
namespace Trd::Impl {
    std::expected<std::filesystem::path, Trd::Err::TrdError> createTemporaryFile();
    std::expected<AuxiliaryStat, Trd::Err::TrdError> getAuxiliaryStat(const std::filesystem::path& path);
    std::expected<PortableStat, Trd::Err::TrdError> statObject(const std::filesystem::path& file);

    inline std::expected<PortableStat, Trd::Err::TrdError> statFile(const std::filesystem::path& file) {
        if (!std::filesystem::is_regular_file(file)) {
            return std::unexpected(Err::TrdError{Err::Code::ObjectNotFile});
        }
        return statObject(file);
    }

};