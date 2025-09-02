
#include <string.h>
#include <iostream>
#include "libtrident.h"
using namespace LibTrident;
using namespace LibTrident::Header;

void print_header(LibTrident::Header::TRD_HEADER& hdr) {
    dbgprintf("[HEADER_START]\n");
    dbgprintf("\tMagic: ");
    for (auto const& it: hdr.magic) {
        dbgprintf("%X ", it);
    }
    dbgprintf("\n");
    dbgprintf("\tExtened Signature: 0x%X\n",hdr.exSignature);
    dbgprintf("\tVersion Format %s\n", 
    PackageHeader::HeaderVersionFormatToString(hdr.fmtVersion).c_str());
    dbgprintf("\tCompression: %u\n", hdr.compression);
    dbgprintf("\tBuild flags %u\n", hdr.buildFlags);
    dbgprintf("\tArchitecture %u\n", hdr.architecture);
    dbgprintf("\tChecksum 0x%X\n", hdr.hdrChksum);
    dbgprintf("\tFile length 0x%lXB\n", hdr.fileLen);
    dbgprintf("\tIoControl 0x%X\n", hdr.ioCtrl);
    dbgprintf("[HEADER_END]\n");
}


int main(void) {
   
    LibTrident::TrPkg lt;
    lt.OpenPackage("/home/shadow/Projects/trident-pm/test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);
    std::cout << "Last Error: " << lt.e << std::endl;
    
    LibTrident::Header::TRD_HEADER h;
    LibTrident::Header::PackageHeader x(lt.fstrInfo);


    x.CreateNewHeader(h, BF_PLATF_LINUX, ARCHT_AM64, COMMPRALG_NONE);

   // std::cout << lt.fstrInfo->GetFileStreamInfo()->name;
    
    
    print_header(h);
    std::cout << x.WriteHeader(h);


    return 0;
}
