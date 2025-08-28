
#include <string.h>
#include <iostream>
#include "libtrident.h"
using namespace LibTrident;

void print_header(LibTrident::TRD_HEADER& hdr) {
    dprintf("[HEADER_START]\n");
    dprintf("\tMagic: ");
    for (auto const& it: hdr.magic) {
        dprintf("%X ", it);
    }
    dprintf("\n");
    dprintf("\tExtened Signature: 0x%X\n",hdr.exSignature);
    dprintf("\tVersion Format %s\n", 
    PackageHeader::HeaderVersionFormatToString(hdr.fmtVersion).c_str());
    dprintf("\tCompression: %u\n", hdr.compression);
    dprintf("\tBuild flags %u\n", hdr.buildFlags);
    dprintf("\tArchitecture %u\n", hdr.architecture);
    dprintf("\tChecksum 0x%X\n", hdr.hdrChksum);
    dprintf("\tFile length 0x%lXB\n", hdr.fileLen);
    dprintf("\tIoControl 0x%X\n", hdr.ioCtrl);
    dprintf("[HEADER_END]\n");
}


int main(void) {
   
    LibTrident::TrPkg lt(LT_INITFL_DEFAULT);
    lt.OpenPackage("/home/shadow/Projects/trident-pm/test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);
    std::printf("%u\n",lt.e.GetError());
    std::cout << lt.e.GetErrorAsString() << '\n';
    LibTrident::TRD_HEADER h;
    LibTrident::PackageHeader x;
    x.CreateNewHeader(h);
    
    print_header(h);


    return 0;
}
