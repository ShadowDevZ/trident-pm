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

        Reserved = 0b1111
    };
    //higher 4 bits reserved for now
    //  enum ValueDataReserved : u8 {
    //      Todo = 0b0000
    //  };

    //for security descriptor
    //todo probably for  KEY/ENTRY and value add 2 separate permissions
    enum class PermissionFlags : Trd::u8 {
        //for all, set always as default, cant be unset
        ReadOnly,
        //for all, read access
        Read = ReadOnly,
        //for keys/entries, new data can be written. Required for creation of subkeys
        CreateNew = 1 << 1,
        //for all, existing data can be edited/renaned, addition of new data is not permitted
        Editable = 1 << 2,
        //any change beyond read operation is denied automatically and this value must NOT be changed
        //unless ChangePermissions is set
        LockPermissions = 1 << 3,
        //Every operation is permitted
        AllAccess = 1 << 4
    };
    class TregAccess {
      private:
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

        bool isPermsLocked() const {
            return permExists(PermissionFlags::LockPermissions);
        }
        //note this whole class doesnt prevent changing permissions if they are locked
        //as this class is not used directly
        void appendPermission(PermissionFlags perms);
        void removePermission(PermissionFlags perms);
        void clearPermissions();
        bool permExists(PermissionFlags perms) const;
        void setAccessVdt(ValueDataTemplate vdt);
        static ValueDataTemplate getAccessVdt(access_word v);
        static u8 getAccessReserved(access_word v);
        static PermissionFlags getAccessPerms(access_word v);
    };
};
