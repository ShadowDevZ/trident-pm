#include "libtrident.h"
#include <iostream>
#include "ioflags.h"
#include <fstream>
#include <sys/stat.h>
#include "fileOperations.h"
using namespace LibTrident;



bool LibTrident::TrPkg::OpenPackage(std::string path, IO_OpenFlag openFlags) {
    std::ios::openmode openMode = IOFLAGS::IOFlags2FsBase(openFlags);
    if (openMode == 0) {
        e.SetError(LTSTATUS::BADARG);
        return false;
    }
    std::string dirPath = Utilities::FileOperations::GetFileDir(path);
    if (dirPath == "") {
        return false;
   }
    //for future use like writing locks in the same directory
    LTSTATUS::LTSTATUS status = Utilities::FileOperations::FileOrDirExists(dirPath, false);
    if (status != LTSTATUS::SUCCESS) {
        return status;
    }
    


    //the user doesnt need to specify 
    openMode |= std::ios::binary;
    
    std::shared_ptr<std::fstream> fsPkg = std::make_shared<std::fstream>(path, openMode);
    if (!fsPkg || !fsPkg->is_open()) {
        e.SetError(LTSTATUS::FOPEN);
        return false;
    }
    
    
    std::streampos fileSize = Utilities::FileOperations::GetFstreamSize(fsPkg);
    if (fileSize == -1) {
        dbgprintf("fsize=-1\n");
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    TRDFilStreameInfo fInfo;
    
    fInfo.fileFlags = openFlags;
    fInfo.fSize = fileSize;

   // fInfo.hFile->seekg(0, std::ios::beg); 
    fInfo.seekOffset = fsPkg->tellg();
    dbgprintf("Seek offset %lu\n", static_cast<u64>(fInfo.seekOffset)); 
    dbgprintf("File size %luB\n", static_cast<u64>(fInfo.fSize)); 
   
    fInfo.hFile = fsPkg;
    fInfo.dirPath = dirPath;
    fInfo.name = path;
    
    if (!fstrInfo->SetFileStreamInfo(fInfo)) {
        e.SetError(fstrInfo->e.GetError());
        return false;
    }

    
    e.Success();
    return true;
}
