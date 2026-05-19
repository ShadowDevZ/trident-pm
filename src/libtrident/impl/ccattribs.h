#pragma once
#include <stdlib.h>

#define __UNMANGLE extern "C"

/**
 * @brief Hint for developer to consider using alternative API call
 * as this API function may produce unsafe output when not handled properly
 * or may result in unexpected results
 * 
 */
#define TRD_UNSAFE_API

/**
 * @brief Hint for developer that the specified field should NOT be edited
 * manually as its automatically assigned by the corresponding function
 * altering this field could result in corruption or errors
 * 
 */
#define TRD_AUTOFIELD

/**
 * @brief Hint for developer that the function marked with this macro is available only in debug mode
 * and cannot be present in release mode
 * 
 */
#define TRD_DBG_BUILD_ONLY

#define LT_INITFL_DEFAULT 1 << 1

#ifdef __DEBUG_PROJ_TARGET
/**
     * @brief Macro to check if the current built is set to Debug mode
     * May output unnecessary debug information
     * do not edit this field manually use './build.sh regen debug' or release
     * Never publish your 
     */
#define _LIBTRIDENT_DEBUG 1
/**
     * @brief Allows more verbose debug output and prints
     */
//#define _LIBTRIDENT_DEBUG_VERBOSE
#endif

#ifdef _LIBTRIDENT_DEBUG
#define dbgprintf(...) fprintf(stderr, __VA_ARGS__)
/*
    Allows to manually overridce endianness for debugging purposes.
    Do not use unless you know what you are doing

    0: Off, no enforcement [default]
    1: Force little endian
    2: Force big endian

    */
#define LT_DEBUG_ENDIAN_FORCE 0
/**
 * @brief Forces the stream output to be passed as chunks to the tstream
 * even for small functions
 * 0: off
 * 1: on
 */
#define LT_TSTREAM_ALWAYS_BUFFER 0

#else
#define dbgprintf(...)                                                                             \
    do {                                                                                           \
    } while (0);

#endif
