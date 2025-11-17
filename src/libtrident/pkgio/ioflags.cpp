#include "ioflags.h"
using namespace LibTrident;
//todo rework this completely, bad code
std::ios::openmode IOFLAGS::IOFlags2FsBase(IO_OpenFlag flags) {
    std::ios::openmode mode = static_cast<std::ios::openmode>(0);
    if (flags & X_LOCK_FILE || flags & X_NOTIMESTAMP) {
        return mode;
    }
    if (flags & CREATE_NEW) mode |= std::ios::out | std::ios::trunc;
    if (flags & ACCESS_R) mode |= std::ios::in;
    if (flags & ACCESS_W) mode |= std::ios::out;
    if (flags & TRUNCATE) mode |= std::ios::trunc;
    if (flags & SEEKPOS_END) mode |= std::ios::ate;
    if (flags & BINFMT) mode |= std::ios::binary;
   
   
    
    return mode;

}
IO_OpenFlag IOFLAGS::FsToIOFlags(std::ios::openmode mode) {
    IO_OpenFlag flags = 0;

    
    if ((mode & std::ios::out) && (mode & std::ios::trunc)) flags |= CREATE_NEW;
    if (mode & std::ios::in)  flags |= ACCESS_R;
    if (mode & std::ios::out) flags |= ACCESS_W;
    if (mode & std::ios::trunc) flags |= TRUNCATE;
    if (mode & std::ios::ate)   flags |= SEEKPOS_END;
    if (mode & std::ios::binary) flags |= BINFMT;

    

    return flags;
}