#pragma once
#include <stdint.h>
#include "ioflags.h"
#include <memory>
#include <fstream>
//basic datatypes
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint32_t uint;
typedef unsigned char uchar;


typedef struct {
    std::unique_ptr<std::ifstream> hIn;
    std::unique_ptr<std::ofstream> hFOut;
    std::streampos seekOffset;
    std::streamsize fSize;
    uint fileFlags;

}FileInfo;