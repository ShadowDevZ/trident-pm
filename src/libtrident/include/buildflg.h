#include "libtrident.h"

typedef enum {
    /*which platform was package designed for*/
    TRD_BF_PLATF_LINUX = 1 << 1,
    TRD_BF_PLATF_NT = 1 << 2,
    TRD_BF_PLATF_ANY = 1 << 3,

    /*what endianness is file encoded in*/
    TRD_BF_FMT_LE = 1 << 4,
    TRD_BF_FMT_BE = 1 << 5,

    /*flag to determine if the package is production ready*/
    TRD_BF_DEBUG = 1 << 6,
    /*architectural platform*/
    TRD_BF_AP_AMD64 = 1 << 7,
    TRD_BF_AP_I386 = 1 << 8,
    TRD_BF_AP_AARCH64 = 1 << 9,
    TRD_BF_AP_ANY = 1 << 10

}TRD_BUILD_FLAG;