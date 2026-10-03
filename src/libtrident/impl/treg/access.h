#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
namespace Trd::Impl {
    //together all these fields should stored up to 2 bytes all data is encoded as series of bits in LE
    //determiens how the stored data should be handled. For example if DtblOffset is set we then
    //expect the set value to be valid file offset
    //lower 4 bits
    //VDT
    enum class ValueDataTemplate : Trd::u8 {

        GenericData = 0b0000,
        DtblOffset = 0b0001,
        //digital signatures and hashes
        CryptoData = 0b0010,
        //Data may be excluded from next rewrite operation as it no longer contains new data
        Temporary = 0b0011,
        DebugData = 0b0100,
        Symlink = 0b0101,
        ExtendedMetadata = 0b0110,
        //only valid for key/entry
        Directory = 0b0111,

        Reserved = 0b1111
    };
    //higher 4 bits reserved for now
    //  enum ValueDataReserved : u8 {
    //      Todo = 0b0000
    //  };

    //for security descriptor
    //todo probably for  KEY/ENTRY and value add 2 separate permissions
    //PFL
    enum class PFDirectory : Trd::u8 {
        ReadOnly,
        Read = ReadOnly,
        CreateSubKey = 1 << 1,
        EditSubKey = 1 << 2,
        EditKey = 1 << 3,
        CreateValue = 1 << 4,
        LockPermissions = 1 << 5,
        AllAccess = (1 << 8) - 1
    };
    enum class PFValue : Trd::u8 {
        //for all, set always as default, cant be unset
        ReadOnly,
        //for all, read access
        Read = ReadOnly,

        EditContent = 1 << 1,

        EditDatatype = 1 << 2,
        EditValue = EditContent | EditDatatype,

        Deletable = 1 << 3,
        //any change beyond read operation is denied automatically and this value must NOT be changed
        //unless ChangePermissions is set
        LockPermissions = 1 << 4,
        //Every operation is permitted
        AllAccess = (1 << 8) - 1
    };
    //Permission control for treg entities
    /* example layout consisting of 2 bytes
        +-----+-----+----------+
        |VDT  |RSV  |   PF     |
        |NBLO |NBHI |HIGHBYTE  |
        |0100 |0000 |00001100  |
        +-----+-----+----------+
    */
    class TregAccess {
      protected:
        access_word access{0};

      public:
        TregAccess(access_word acword) {
            setAccessWord(acword);
        };
        TregAccess() = default;

        access_word getAccessWord() const {
            return access;
        }
        void setAccessWord(access_word word) {
            access = word;
        }

        // bool isPermsLocked() const {
        //     return permExists(PFDirectory::LockPermissions);
        // }
        //note this whole class doesnt prevent changing permissions if they are locked
        //as this class is not used directly
        void clearPermissions();

        void setAccessVDT(ValueDataTemplate vdt);
        static ValueDataTemplate getAccessVdt(access_word v);

      protected:
        void appendFlagPFL(u8 f);
        void clearFlagPFL(u8 f);
        bool existsFlagPFL(u8 perms) const;
        static u8 getReservedNibble(access_word v);
        static u8 getPFL(access_word v);
    };
    class TregValAccess : public TregAccess {
      public:
        TregValAccess(access_word acword) {
            setAccessWord(acword);
        };
        TregValAccess() = default;
        void appendPermission(PFValue perms) {
            appendFlagPFL(static_cast<u8>(perms));
        }
        void removePermission(PFValue perms) {
            clearFlagPFL(static_cast<u8>(perms));
        }
        void permExists(PFValue perms) {
            existsFlagPFL(static_cast<u8>(perms));
        }
        static PFValue getAccessPerms(access_word v) {
            return static_cast<PFValue>(getPFL(v));
        }
    };
    class TregDirAccess : public TregAccess {
      public:
        TregDirAccess(access_word acword) {
            setAccessWord(acword);
        };
        TregDirAccess() = default;
        void appendPermission(PFDirectory perms) {
            appendFlagPFL(static_cast<u8>(perms));
        }
        void removePermission(PFDirectory perms) {
            clearFlagPFL(static_cast<u8>(perms));
        }
        void permExists(PFDirectory perms) {
            existsFlagPFL(static_cast<u8>(perms));
        }
        static PFDirectory getAccessPerms(access_word v) {
            return static_cast<PFDirectory>(getPFL(v));
        }
    };
};
