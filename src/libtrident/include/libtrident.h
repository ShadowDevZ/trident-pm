#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>
#define __STRUCT_PACK __attribute__((packed))
#define PACKED_STRUCT typedef struct __attribute__((packed))
#include "trderr.h"
#include "pkgio.h"
#include "ioflags.h"
#include <sys/stat.h>
#define __UNMANGLE extern "C"


#define LT_INITFL_DEFAULT 1 << 1

#define _LIBTRIDENT_DEBUG 1

#ifdef _LIBTRIDENT_DEBUG
#define dprintf(...) fprintf( stderr, __VA_ARGS__ )
#else
#define dprintf(...) do{ } while ( 0 )
#endif



class LibTrident  {
public:
    LTSTATUS::TridentError e;

    LibTrident(int iFlags) {
        initFlags = iFlags;
    };
    bool OpenPackage(std::string path, IO_OpenFlag openFlags);

private:
    int initFlags;
    FileInfo fInfo;

};
