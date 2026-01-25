#include "ioflags.h"
#include <optional>

using namespace LibTrident;
using namespace LibTrident::Impl;

std::optional<std::ios::openmode> Impl::translateAccessModel(const TRDAccessModel& accessModel) {
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

