#pragma once
#include "datatypes.h"
namespace LibTrident::Consts::HeaderConsts {
    //when printing dont forget to add NULL terminator
    constexpr byte TRD_HDR_MAGIC[] = {
        0x93, 0x54, 0x52, 0x44, 0x21, 0x12, 0x2E, 0x53
    };//\223TRD!\x12.S
    constexpr u16 TRD_HDR_EXTENDED_SIGNATURE = 0xbf97;
    constexpr u8 TRD_HDR_VMAJOR = 1;
    constexpr u8 TRD_HDR_VMINOR = 0;
    constexpr u8 TRD_HDR_VREVISION = 0;


    constexpr u32 TRD_HDR_START_OFFSET = 0;

    constexpr u16 LT_HDR_SZB_01A = 32;
};
//this is taken from TRPX docs, better way would of course be to use sizeof
namespace LibTrident::Consts::SD {
    constexpr u16 TRD_SECTIONSD_SIZE = 32;
};
namespace LibTrident::Consts::SUID {
    constexpr u8 SUID_MAX_LENGTH = 36;
};

