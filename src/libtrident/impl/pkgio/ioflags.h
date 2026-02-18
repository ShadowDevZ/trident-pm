/**
 * @file ioflags.h
 * @brief IO flags used when opening package and provide access to sections
 * 
 * 
 */
#pragma once
#include "datatypes.h"
#include <optional>
#include <memory>
namespace Trd {

   /*
    enum FileFlags : IO_OpenFlag {
        OCREATE_NEW       = 1 << 1,
        OPEN_EXISTING    = 1 << 2,
        ACCESS_R         = 1 << 3,
        ACCESS_W         = 1 << 4,
        X_LOCK_FILE       = 1 << 5,
        X_NOTIMESTAMP     = 1 << 6,
        //INTERNAL ONLY, DO NOT SET UNDER ANY CIRCUMSTANCE
        _I_IO_INVCLOSED   = 1 << 10,
        // Useless
       // TRUNCATE         = 1 << 7,
      //  SEEKPOS_END      = 1 << 8,
      //  SEEKPOS_START    = 0,
      //  BINFMT           = 1 << 9,
     //

        
        ACCESS_RW        = ACCESS_R | ACCESS_W
        
    };
    */
    
    /**
     * @brief Package open type
     * 
     */
    enum class TrdOpenIO : u8 {
        /// none or default
        None,
        /// always creates new file overwriting the old one if present
        CreateNew,
        /// creates file in the system specific temporary directory
        CreateTemporary,
        /// opens the existing file
        OpenExisting,
        /// creates package from array of bytes or partial memory block
        CreateFromMemBuff

    };

    /**
     * @brief Access to package
     * 
     */
    enum class TrdAccessIO : u8 {
        None,
        ReadAll = 1 << 1,
        WriteAll = 1 << 2,
        AllAccess = ReadAll | WriteAll,
        /*
        allows for reading only certain regions like .treg, 
        user can now call something like ReadRegionTreg() which doesnt for example
        need to map whole .data to the memory which could be gigabytes in size
        Idea is that what regions are accessed depends entirely on something like
        GetRegionAccess(REGION_TREG|REGION_SD,...)
        */
        ReadRegion = 1 << 3,
        WriteRegion = 1 << 4
    };
    /**
     * @brief Extended attributes
     * 
     */
    enum class TrdXattrIO : u8 {
        /// no extended attributes, default
        None,
        /// locks the file and prevents other programs from reading and writing the data whilst open
        LockFile = 1 << 1,
        /// Do not change the modification time
        NoTimeStamp = 1 << 2, 
        
    };
    /**
     * @brief Internal flags for library use
     * 
     */
    enum class _TrdInternalIO : u8 {
        //reserved internal, do not use
        None = 0,
        IoOpen = None,
        IoClosed = ((1 << 8) - 1)

    };
    struct TRDAccessModel{
        TrdOpenIO open {TrdOpenIO::None};
        TrdAccessIO access {TrdAccessIO::None};
        TrdXattrIO xattr {TrdXattrIO::None};
        _TrdInternalIO _internal {_TrdInternalIO::None};
    };

    namespace Impl {
    /*Translates AccessModel into std::ios:openmode. If mode cannot be translated like for example
    TrdAccessIO::CreateFromMembuff returns nullopt. For attributes like TrdXattrIO or _TrdReserved nothing happens
    TrdOpenIO and TRDAccessIO are mandatory.
    */
    std::optional<std::ios::openmode> translateAccessModel(const TRDAccessModel& accessModel); 
     //all attributes without X prefix can be translated
     //if an attribute with X prefix is passed, returns 0;
   // std::ios::openmode IOFlags2FsBase(IO_OpenFlag flags);
   // IO_OpenFlag FsToIOFlags(std::ios::openmode mode);
    };
}