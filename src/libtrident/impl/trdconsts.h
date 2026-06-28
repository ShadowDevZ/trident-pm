/**
 * @file trdconsts.h
 * @brief Constants used throughout the library
 * @todo Improve the naming of constants and namespaces 
 * 
 * 
 */
//THIS WHOLE FILE IS A MESS WITH NAMES PORTED FROM C MACROS FIX THIS
//we seriouslly need an overhaul of this mess
#pragma once
#include "datatypes.h"
namespace Trd::Consts {

    inline constexpr u32 TRD_INVALID_CHKSUM = 0;
};

namespace Trd::Consts::Header {
    //when printing dont forget to add NULL terminator
    /// TRPX magic number
    inline constexpr std::byte TRD_HDR_MAGIC[] = {
        std::byte{0x93}, std::byte{0x54}, std::byte{0x52}, std::byte{0x44},
        std::byte{0x21}, std::byte{0x12}, std::byte{0x2E}, std::byte{0x53}}; //\223TRD!\x12.S
    /// extended signature for additional checking
    inline constexpr u16 TRD_HDR_EXTENDED_SIGNATURE = 0xbf97;

    inline constexpr u16 TRD_HDR_INVALID_VERSION = 0;
    /// bleeding edge header version for packages pulled directly from master branch
    inline constexpr u16 TRD_HDR_BLEEDING_EDGE = UINT16_MAX;
    //Package major, minor version and revision

    /// major header version
    inline constexpr u8 TRD_HDR_VMAJOR = 1;
    /// minor header version
    inline constexpr u8 TRD_HDR_VMINOR = 0;
    /// header revision
    inline constexpr u8 TRD_HDR_VREVISION = 0;

    /// file seek offset pointing where to start writing header
    inline constexpr u32 TRD_HDR_START_OFFSET = 0;
    /// size of header in bytes according to the specification
    inline constexpr u16 LT_HDR_SZB_01A = 32;
};

namespace Trd::Consts::SD {
    /// size of SD section as defined by the documentation
    inline constexpr u16 TRD_SECTIONSD_SIZE = 64;
    inline constexpr std::byte TRD_SD_IDBYTE = std::byte{0x5D};
};
namespace Trd::Consts::SUID {
    /// maximum length of the SUID string containing the predefined UUID of a specific section without NULL terminator
    inline constexpr u8 SUID_MAX_LENGTH = 40;
};
namespace Trd::Consts::Binary {
    /**
     * @brief data alignment size in bytes when manipulating TStreams
     * @details alignemnt is by default 8 for 64 bit platforms
     * @todo for 32 bit platforms we should probably want something like 4 but that would make the packages
     * written for 64 bit incompatible
     */
    inline constexpr u16 BSERIALIZE_DATA_ALIGN = 8;
    inline constexpr u32 IO_CHUNK_SIZE = 256 * 1024; // 256KiB;
}

namespace Trd::Consts::Err {
    /// maximum size of the user specified extended error message
    inline constexpr u16 SECONDARY_ERROR_MAXSIZE = 256;
};