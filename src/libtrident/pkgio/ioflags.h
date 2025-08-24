#pragma once
#include "datatypes.h"


namespace IOFLAGS {

    
    enum FileFlags : uint {
        CREATE_NEW       = 1 << 1,
        OPEN_EXISTING    = 1 << 2,
        ACCESS_R         = 1 << 3,
        ACCESS_W         = 1 << 4,
        LOCK_FILE        = 1 << 5,
        OPEN_NOTIMESTAMP = 1 << 6,
        ACCESS_RW        = ACCESS_R | ACCESS_W
    };

}