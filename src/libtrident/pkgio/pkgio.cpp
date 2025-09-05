#include "pkgio.h"
#include <sys/stat.h>
#include <stdio.h>
#include "ccattribs.h"
using namespace LibTrident;
using namespace PkgIO;
std::string FileOperations::GetFileDir(const std::string& file) {
    size_t pos = file.find_last_of('/');
    if (pos == std::string::npos) {
        return "";
    }
    return file.substr(0, pos);
}
std::streamsize FileOperations::GetFstreamSize(std::shared_ptr<std::fstream> fs) {
    if (!fs || !fs->is_open()) {
        return -1;
    }
    return GetFstreamSize(*fs);
}

std::streamsize FileOperations::GetFstreamSize(std::fstream& fs) {
    if (!fs.is_open()) {
        return -1;
    }
    std::streampos orignalPosition = fs.tellg();
    std::streampos fileSize;
    if (orignalPosition == -1) {
        return -1;
    }
    fs.seekg(0, std::ios::end);
    if (fs.fail()) {
         goto fail;
         
    }
    fileSize = fs.tellg();
    if (fileSize == -1) {
        goto fail;
    }
    //reset to the original state
    fs.seekg(orignalPosition, std::ios::beg);
    return fileSize;

fail:
    fs.seekg(orignalPosition, std::ios::beg);
    return -1;

  
}

LibTrident::LTSTATUS::LTSTATUS FileOperations::FileOrDirExists(const std::string& path, bool file) {
    
    //handle scenarios like /tmp/folder/
    std::string fullPath = path;
    if (!file) {
        if(fullPath.back() == '/') {
            size_t pos = path.find_last_of("/");
            std::string fullPath  = path.substr(0, pos);
            fullPath = path.substr(0, pos - 1);
    }
}
     struct stat dStat;
    if (stat(fullPath.c_str(), &dStat) != 0) {
      //  e.SetError(LTSTATUS::INVFILE);
        return LTSTATUS::INVFILE;
    }
    if (!file) {
        if (!S_ISDIR(dStat.st_mode)) {
             dbgprintf("stat(D_%s) fail\n", fullPath.c_str());
            return LTSTATUS::NOTDIR;
        }
    }
    else {
        if (!S_ISREG(dStat.st_mode)) {
            dbgprintf("stat(R_%s) fail\n", fullPath.c_str());
            return LTSTATUS::NOTFILE;
        }
    }
   

    return LTSTATUS::SUCCESS;
}