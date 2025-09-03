#pragma once
#include "datatypes.h"


namespace LibTrident::IOFLAGS {

   
    enum FileFlags : IO_OpenFlag {
        CREATE_NEW       = 1 << 1,
     //   OPEN_EXISTING    = 1 << 2,
        ACCESS_R         = 1 << 3,
        ACCESS_W         = 1 << 4,
        X_LOCK_FILE       = 1 << 5,
        X_NOTIMESTAMP     = 1 << 6,
        TRUNCATE         = 1 << 7,
        SEEKPOS_END      = 1 << 8,
        SEEKPOS_START    = 0,
        BINFMT           = 1 << 9,
        //INTERNAL ONLY, DO NOT SET UNDER ANY CIRCUMSTANCE
        _I_IO_INVCLOSED   = 1 << 10,

        
        ACCESS_RW        = ACCESS_R | ACCESS_W
        
    };
    
     //all attributes without X prefix can be translated
     //if an attribute with X prefix is passed, returns 0;
    std::ios::openmode IOFlags2FsBase(IO_OpenFlag flags);
    IO_OpenFlag FsToIOFlags(std::ios::openmode mode);

}
