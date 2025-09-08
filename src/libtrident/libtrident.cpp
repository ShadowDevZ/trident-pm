#include "libtrident.h"
#include <iostream>
#include "ioflags.h"
#include <fstream>
#include <sys/stat.h>
using namespace LibTrident;
using namespace PkgIO;

bool LibTrident::TrPkg::ClosePkg() {
    if (!fstrInfo->CheckFileStreamInfo()) {
        dbgprintf("Error closing stream\n");
        return false;
    }
    std::shared_ptr<FstreamInfo::TRDFilStreameInfo> closeInfo =  fstrInfo->GetFileStreamInfo();
   
    closeInfo->dirPath = "";
    closeInfo->name = "";
    fstrInfo->CloseStream();
    closeInfo->hFile->close();
     dbgprintf("Stream closed\n");
    return true;
}


bool LibTrident::TrPkg::OpenPackage(const std::string& path, IO_OpenFlag openFlags) {
    std::ios::openmode openMode = IOFLAGS::IOFlags2FsBase(openFlags);
    if (openMode == 0) {
        e.SetError(LTSTATUS::BADARG);
        return false;
    }
    std::string dirPath = FileOperations::GetFileDir(path);
    
    if (dirPath == "") {
        e.SetError(LTSTATUS::NOTDIR);
        return false;
   }
    //for future use like writing locks in the same directory
    LTSTATUS::LTSTATUS status = FileOperations::FileOrDirExists(dirPath, false);
    if (status != LTSTATUS::SUCCESS) {
        e.SetError(LTSTATUS::NOTDIR);
        return status;
    }
    


    //the user doesnt need to specify 
    openMode |= std::ios::binary;
    
    std::shared_ptr<std::fstream> fsPkg = std::make_shared<std::fstream>(path, openMode);
    if (!fsPkg || !fsPkg->is_open()) {
        e.SetError(LTSTATUS::FOPEN);
        return false;
    }
    
    
    std::streampos fileSize = FileOperations::GetFstreamSize(fsPkg);
    if (fileSize == -1) {
        dbgprintf("fsize=-1\n");
        e.SetError(LTSTATUS::FSEEK);
        return false;
    }
    FstreamInfo::TRDFilStreameInfo fInfo;
    
    fInfo.fileFlags = openFlags;
    fInfo.fSize = fileSize;

   // fInfo.hFile->seekg(0, std::ios::beg); 
    fInfo.seekOffsetRead = static_cast<std::streampos>(0);
    fInfo.seekOffsetWrite = static_cast<std::streampos>(0);
    dbgprintf("Seek offset %lu\n", static_cast<u64>(fInfo.seekOffsetRead)); 
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
//ClosePkg, reset context, filestream close fd