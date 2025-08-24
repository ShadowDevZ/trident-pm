#pragma once
#include <stdint.h>

namespace LTSTATUS {


    typedef uint32_t LTSTATUS;

    typedef enum {
            OK,
            FAIL,
            MALLOC,
            BADARG,
            ACCESS,
            ALR_INIT,
            NULL_OBJ,
            FOPEN
    }RSP;
}
