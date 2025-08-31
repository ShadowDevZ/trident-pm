#pragma once
#include <cstdint>

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
typedef uint32_t IO_OpenFlag;
typedef unsigned char byte;

typedef struct {
    std::string dirPath;
    std::string name;
    std::shared_ptr<std::fstream> hFile;
    std::streampos seekOffset;
    std::streamsize fSize;
    IO_OpenFlag fileFlags;

}TRDFilStreameInfo;
#include "ccattribs.h"