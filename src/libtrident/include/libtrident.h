#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>
#define __STRUCT_PACK __attribute__((packed))
#include "trderr.h"
#include "pkgio.h"
#include "ioflags.h"
#define __UNMANGLE extern "C"


class LibTrident {
public:
    int GetValue();
    LibTrident(int iFlags) {
        vv = iFlags;
    };
    LTSTATUS::LTSTATUS OpenPackage(std::string path, IOFLAGS::FileFlags openFlags);

private:
    int vv;
    FileInfo fInfo;

};
