#include "ioflags.h"
#include <optional>
using namespace LibTrident;
using namespace LibTrident::IOFLAGS;

std::optional<std::ios::openmode> IOFLAGS::TranslateAccessModel(const TRDAccessModel& accessModel) {
    //if we don't provide default value we cannot use OR. 
    //Unfortunately ios::openmode doesn't contain 0 value
    std::ios::openmode iosOpen = static_cast<std::ios::openmode>(0); //NOLINT
    switch (accessModel.open)
    {
    case TrdOpenIO::CreateNew:
        [[fallthrough]];
    case TrdOpenIO::CreateTemporary:
        iosOpen |= std::ios::trunc | std::ios::out;
        break;
    case TrdOpenIO::OpenExisting:
        iosOpen |= std::ios::out;
        break;
    case TrdOpenIO::None:
        [[fallthrough]];
        //don't know if i should leave it there or remove it
    case TrdOpenIO::CreateFromMemBuff:
        [[fallthrough]];
    default:
        return std::nullopt;
    }

    switch (accessModel.access)
    {
    case TrdAccessIO::ReadAll:
        [[fallthrough]];
    case TrdAccessIO::ReadRegion:
        iosOpen |= std::ios::in;
        break;
    case TrdAccessIO::WriteAll:
        [[fallthrough]];
    case TrdAccessIO::WriteRegion:
        iosOpen |= std::ios::out;
        break;
    case TrdAccessIO::AllAccess:
        iosOpen |= std::ios::in | std::ios::out;
        break;
    case TrdAccessIO::None:
        [[fallthrough]];
    default:
        return std::nullopt;
    }
    //xattrs and reserved are ignored as they cant be translated
    return iosOpen;
    
}


//todo rework this completely, bad code
/*
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
    */

