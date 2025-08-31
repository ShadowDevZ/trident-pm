#include "libtrident.h"
#include <iostream>
#include "ioflags.h"
#include <fstream>
#include <sys/stat.h>


bool LibTrident::TrPkg::OpenPackage(std::string path, IO_OpenFlag openFlags) {
    std::ios::openmode openMode = IOFLAGS::IOFlags2FsBase(openFlags);
    if (openMode == 0) {
        e.SetError(LTSTATUS::BADARG);
        return false;
    }
    size_t pos = path.find_last_of("/");
   std::string tmpDirPath  = path.substr(0, pos);
    //handle scenarios like /tmp/folder/
    if(tmpDirPath.back() == '/') {
        tmpDirPath = path.substr(0, pos - 1);
    }
    struct stat dStat;
    if (stat(tmpDirPath.c_str(), &dStat) != 0) {
        e.SetError(LTSTATUS::INVFILE);
        return false;
    }
    if (!S_ISDIR(dStat.st_mode)) {
        e.SetError(LTSTATUS::NOTDIR);
        return false;
    }
    dprintf("dirstat exists %s\n", tmpDirPath.c_str());


    //the user doesnt need to specify 
    openMode |= std::ios::binary;
    
    fInfo.hFile = std::make_shared<std::fstream>(path, openMode);
    if (!fInfo.hFile || !fInfo.hFile->is_open()) {
        e.SetError(LTSTATUS::FOPEN);
        return false;
    }
    fInfo.fileFlags = openFlags;
   
   // fInfo.fSize = fInfo.hFile->tellg();

   // fInfo.hFile->seekg(0, std::ios::beg); 
    fInfo.seekOffset = fInfo.hFile->tellg();
    dprintf("Seek offset %lu\n", static_cast<u64>(fInfo.seekOffset)); 
   // dprintf("File size %luB\n", static_cast<u64>(fInfo.fSize)); 

   
   fInfo.dirPath = tmpDirPath;

   fInfo.name = path.substr(pos+1);
  
    e.SetError(LTSTATUS::SUCCESS);
    return true;
}
