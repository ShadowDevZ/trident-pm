#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>



#include "trderr.h"
#include "pkgio.h"
#include "ioflags.h"
#include <sys/stat.h>
#include "trheader.h"
namespace LibTrident {




class TrPkg  {
public:
    //works 
    LTSTATUS::TridentError e;
    //doesnt work
    LibTrident::Header::PackageHeader hdr;
   // PackageHeader pkg;
   
   
   // PackageHeader hdr;
    

    TrPkg(int iFlags) {
        initFlags = iFlags;
    };
    bool OpenPackage(std::string path, IO_OpenFlag openFlags);

private:
    int initFlags;
    TRDFilStreameInfo fInfo;

};
};