
#include <string.h>
#include <iostream>
#include "libtrident.h"

int main(void) {
   
    LibTrident lt(LT_INITFL_DEFAULT);
    lt.OpenPackage("/home/shadow/Projects/trident-pm/test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);
    std::printf("%u\n",lt.e.GetError());
    std::cout << lt.e.GetErrorAsString() << '\n';

    return 0;
}
