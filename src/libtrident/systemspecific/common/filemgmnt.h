#pragma once
#include <optional>
#include "ccattribs.h"
#include <filesystem>
namespace LibTrident::SystemSpecific {
    
    std::optional<std::filesystem::path> CreateTemporaryFile();

};