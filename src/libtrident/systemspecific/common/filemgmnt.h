#pragma once
#include <expected>
#include "ccattribs.h"
#include <filesystem>
#include "trderr.h"
namespace LibTrident::SystemSpecific {
    
    std::expected<std::filesystem::path, LibTrident::Err::TrdError> CreateTemporaryFile();

};