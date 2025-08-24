#include "libtrident.h"
#include <iostream>
#include "ioflags.h"
#include <fstream>

int LibTrident::GetValue() {
    
    return vv;
}
LTSTATUS::LTSTATUS LibTrident::OpenPackage(std::string path, IOFLAGS::FileFlags openFlags) {
   
    fInfo.hIn = std::make_unique<std::ifstream>(path, std::ios::binary | std::ios::ate);
    if (!fInfo.hIn || !fInfo.hIn->is_open()) {
        return LTSTATUS::FOPEN;
    }
    fInfo.fileFlags = openFlags;
   
    fInfo.fSize = fInfo.hIn->tellg();

    fInfo.hIn->seekg(0, std::ios::beg); 
    fInfo.seekOffset = fInfo.hIn->tellg();
    std::cout << "Seek offset" << fInfo.seekOffset << '\n';
    std::cout << "Fsize B " << fInfo.fSize << '\n';
    return LTSTATUS::OK;
}
