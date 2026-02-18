#pragma once
#include "datatypes.h"
#include <filesystem>
namespace Trd::Impl {

    struct AccessTimes
    {
        std::chrono::system_clock::time_point lastAccess;
        std::chrono::system_clock::time_point lastModify;
        std::chrono::system_clock::time_point lastMetadataChange;
        std::optional<std::chrono::system_clock::time_point> fileCreated;
    };
    //info is os specific
    struct AuxiliaryStat {
        AccessTimes times;

        std::optional<u32> optOwnerUID;
        std::optional<u32> optOwnerGID;
    };
    //portable stat information obtained via std::filesystem
    struct PortableStat {
        std::filesystem::file_type fileType;
        std::optional<u64> fileSize;
        std::filesystem::perms permissions;
        //extended non portable information
        std::optional<AuxiliaryStat> auxiliary;


    };
}
