#pragma once
#include "datatypes.h"
namespace LibTrident::Consts::HeaderConsts {
    //when printing dont forget to add NULL terminator
    //TRPX magic number
    constexpr byte TRD_HDR_MAGIC[] = {
        0x93, 0x54, 0x52, 0x44, 0x21, 0x12, 0x2E, 0x53
    };//\223TRD!\x12.S
    //extended signature for additional checking
    constexpr u16 TRD_HDR_EXTENDED_SIGNATURE = 0xbf97;

    //Package major, minor version and revision
    constexpr u8 TRD_HDR_VMAJOR = 1;
    constexpr u8 TRD_HDR_VMINOR = 0;
    constexpr u8 TRD_HDR_VREVISION = 0;

    //file seek offset pointing where to start writing header
    constexpr u32 TRD_HDR_START_OFFSET = 0;
    //size of header in bytes according to the specification
    //if input TRD_HEADER struct < LT_HDR_SZB_0XXXX then we need to append 0's for compatibility
    constexpr u16 LT_HDR_SZB_01A = 32;
};
//this is taken from TRPX docs, better way would of course be to use sizeof
namespace LibTrident::Consts::SD {
    constexpr u16 TRD_SECTIONSD_SIZE = 32;
};
namespace LibTrident::Consts::SUID {
    //maximum length of the SUID string containing the predefined UUID of a specific section without NULL terminator
    constexpr u8 SUID_MAX_LENGTH = 40;
};
namespace LibTrident::Consts::Binary {
    //number of bytes to align, probably should use preprocessor to check if 32 bit to set data align by 4
    //may be needed on certain CPU's to achieve compatibility
    //if no alignment is to be used this constant should be 0
    constexpr u16 BSERIALIZE_DATA_ALIGN = 8;
}

