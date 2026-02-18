/**
 * @file trdconsts.h
 * @brief Constants used throughout the library
 * @todo Improve the naming of constants and namespaces 
 * 
 * 
 */
#pragma once
#include "datatypes.h"
namespace Trd::Consts::Header {
    //when printing dont forget to add NULL terminator
    /// TRPX magic number
    constexpr u8 TRD_HDR_MAGIC[] = {
        0x93, 0x54, 0x52, 0x44, 0x21, 0x12, 0x2E, 0x53
    };//\223TRD!\x12.S
    /// extended signature for additional checking
    constexpr u16 TRD_HDR_EXTENDED_SIGNATURE = 0xbf97;

    constexpr u16 TRD_HDR_INVALID_VERSION = 0;
    /// bleeding edge header version for packages pulled directly from master branch
    constexpr u16 TRD_HDR_BLEEDING_EDGE = UINT16_MAX;
    //Package major, minor version and revision
    constexpr u32 TRD_HDR_INVALID_CHKSUM = 0;
    /// major header version
    constexpr u8 TRD_HDR_VMAJOR = 1;
    /// minor header version
    constexpr u8 TRD_HDR_VMINOR = 0;
    /// header revision
    constexpr u8 TRD_HDR_VREVISION = 0;

    /// file seek offset pointing where to start writing header
    constexpr u32 TRD_HDR_START_OFFSET = 0;
    /// size of header in bytes according to the specification
    constexpr u16 LT_HDR_SZB_01A = 32;
};

namespace Trd::Consts::SD {
    /// size of SD section as defined by the documentation
    constexpr u16 TRD_SECTIONSD_SIZE = 32;
};
namespace Trd::Consts::SUID {
    /// maximum length of the SUID string containing the predefined UUID of a specific section without NULL terminator
    constexpr u8 SUID_MAX_LENGTH = 40;
};
namespace Trd::Consts::Binary {
    /**
     * @brief data alignment size in bytes when manipulating TStreams
     * @details alignemnt is by default 8 for 64 bit platforms
     * @todo for 32 bit platforms we should probably want something like 4 but that would make the packages
     * written for 64 bit incompatible
     */
    constexpr u16 BSERIALIZE_DATA_ALIGN = 8;
}

namespace Trd::Consts::Err {
    /// maximum size of the user specified extended error message
    constexpr u16 SECONDARY_ERROR_MAXSIZE = 256;
};