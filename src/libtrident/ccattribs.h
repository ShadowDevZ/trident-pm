#pragma once
#include <assert.h>
#include <stdlib.h>
#define __STRUCT_PACK __attribute__((packed))
#define PACKED_STRUCT typedef struct __attribute__((packed))
#define __UNMANGLE extern "C"


#define LT_INITFL_DEFAULT 1 << 1

#ifndef __DEBUG_PROJ_TARGET
    #define _LIBTRIDENT_DEBUG 1
#endif
#ifdef _LIBTRIDENT_DEBUG
    #define dbgprintf(...) fprintf( stderr, __VA_ARGS__ )
    #define ASSERTION_FAIL_EXIT_CODE 500

    /*im not using __assert_fail because my system is filled up with core dumps */
    #define dassert(expr) assert(expr);
    #define tassert(fnName, expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "\x1B[31m[fail]%s\nAS: <%s :: %s, %d, %s>\x1B[0m\n", fnName, #expr, __ASSERT_FILE, __ASSERT_LINE, __ASSERT_FUNCTION); \
            exit(ASSERTION_FAIL_EXIT_CODE); \
        } else { \
            std::cout << "\x1B[32m" << fnName << " [ok]\x1B[0m\n"; \
        } \
    } while(0);
    
#else
    #define dbgprintf(...) do{ } while ( 0 );
    #define tassert(x, msg) do{ } while (0);
    #define dassert(expr) do{ } while (0);;
#endif

