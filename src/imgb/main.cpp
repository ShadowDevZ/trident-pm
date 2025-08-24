
#include <string.h>
#include <iostream>
#include "libtrident.h"

int main(void) {
   
    LibTrident lt(132);
    lt.OpenPackage("/home/shadow/Projects/trident-pm/package.tpx", IOFLAGS::ACCESS_R);
    std::printf("empty test %d\n", lt.GetValue());

    return 0;
}
