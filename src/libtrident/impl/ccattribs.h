#pragma once
#include <assert.h>
#include <stdlib.h>
#define __STRUCT_PACK __attribute__((packed))
#define PACKED_STRUCT typedef struct __attribute__((packed))
#define __UNMANGLE extern "C"

#define LT_UNSAFE_API

#define LT_INITFL_DEFAULT 1 << 1

#if defined(__DEBUG_PROJ_TARGET)
    #define _LIBTRIDENT_DEBUG 1
#endif






#if defined(_LIBTRIDENT_DEBUG)
    #define dbgprintf(...) fprintf( stderr, __VA_ARGS__ )
    #define ASSERTION_FAIL_EXIT_CODE 500

        /*
    Allows to manually overridce endianness for debugging purposes.
    Do not use unless you know what you are doing

    0: Off, no enforcement [default]
    1: Force little endian
    2: Force big endian

    */
    #define LT_DEBUG_ENDIAN_FORCE 0
    
#else
    #define dbgprintf(...) do{ } while ( 0 );
  //  #define tassert(x, msg) do{ } while (0);

#endif
