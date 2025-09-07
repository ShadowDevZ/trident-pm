#pragma once
#define __STRUCT_PACK __attribute__((packed))
#define PACKED_STRUCT typedef struct __attribute__((packed))
#define __UNMANGLE extern "C"


#define LT_INITFL_DEFAULT 1 << 1

#ifndef __DEBUG_PROJ_TARGET
    #define _LIBTRIDENT_DEBUG 1
#endif
#ifdef _LIBTRIDENT_DEBUG
    #define dbgprintf(...) fprintf( stderr, __VA_ARGS__ )
#else
    #define dbgprintf(...) do{ } while ( 0 )
#endif
