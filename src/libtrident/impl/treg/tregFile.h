#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"
#include <variant>
#include "access.h"
namespace Trd::Consts::Treg {
    inline constexpr u32 TREG_MAGIC = 0x52444852; //RHDR (BE)
    inline constexpr u16 KEY_REC_ID = 0x526B; //kR (BE)
    inline constexpr u16 ENTRY_REC_ID = 0x5265; //eR (BE)
    inline constexpr u16 VALUE_REC_ID = 0x5276; //vR (BE)
    //stored string is not guaranteed to be null terminated
    inline constexpr u16 KEYNAME_MAXLEN = 0xFFFF;
    /*limit for the string/vector for value, 64KiB, treg is NOT a file storage
    but a metadata registry container, if you need to store large data use DTBL API
    */
    inline constexpr u32 VALUE_MAXSIZE = (64 * 1024);
};

namespace Trd {
    namespace RegFlags {
        enum Flag : u16 {
            Clear = 0
        };
    };
    /*
    trivial data is data storable within 8 bytes
    even though string and vector could be less than 8 bytes they are stored inside pool no matter what
    0-127 Trivial datatypes
    128-255 Non trivial datatypes
    */
    enum class TregValueDatatype : u8 {
        i8 = 0,
        i16,
        i32,
        i64,
        u8,
        u16,
        u32,
        u64,
        u8_bool,
        soffset,
        // tregSymLink, //no implemented
        cstr = 128,
        binLE,
    };

    struct TregHeader : Impl::SerializableData {
        /*
        u32 magic{Consts::Treg::TREG_MAGIC};
        RegFlags::Flag flags{RegFlags::Clear};
        soffset regRootOffset;
        u64 regSizeTotal;
        u64 _reserved;
        u16 _reserved1;
*/

        u32 magic{Consts::Treg::TREG_MAGIC};
        RegFlags::Flag flags{RegFlags::Clear};
        u32 subKeysCount;
        u32 subEntriesCount;
        u32 subValsCount;
        u64 poolSize;
        u32 checksum; //everything after header until the end of treg section
        u16 _reserved1 = 0;

        static constexpr int headerSize() {
            return 32;
        }

        constexpr u64 size() const override {
            constexpr auto x =
                Impl::BinarySerializer::elementSize(magic, flags, subKeysCount, subEntriesCount,
                                                    subValsCount, poolSize, checksum, _reserved1);
            static_assert(x == headerSize(), "Size missmatch");
            return x;
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
        void dbgInfoPrint() const override;
        //doesnt make sense here, only in final TregObject
        //  std::optional<Impl::Crc32Gen> checksumCRC32() const;
    };
    //singular key record
    //all sizes are checked in Key class that fills this record, this class must NOT be used directly
    struct TregKeyRecord : Impl::SerializableData {
        u16 identifier = Consts::Treg::KEY_REC_ID; // just a mark to fill instead of padding
        u32 keyNameOffset; //offset in flat vector
        u16 keyNameLength;
        u32 firstChildKeyIndex;
        u16 childKeysCount;

        u32 firstEntryIndex;
        u16 entryCount;
        Trd::access_word accessWord;
        u32 recordChecksum = Consts::TRD_INVALID_CHKSUM;
        /*
        sum of all fields in bytes, there is probably a way better way to do this but i want this 
        to be static and i dont want to use magic numbers that make no sense
        */
        static constexpr int keyRecordSize() {
            return 26;
        }
        constexpr u64 size() const override {
            constexpr auto x = Impl::BinarySerializer::elementSize(
                identifier, keyNameOffset, keyNameLength, firstChildKeyIndex, childKeysCount,
                firstEntryIndex, entryCount, accessWord, recordChecksum);

            static_assert(x == keyRecordSize(), "Size missmatch");
            return x;
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
        void dbgInfoPrint() const override;
    };
    //all sizes are checked in Entry class that fills this record, this class must NOT be used directly

    struct TregEntryRecord : Impl::SerializableData {
        u16 identifier = Consts::Treg::ENTRY_REC_ID;
        u32 entryNameOffset;
        u16 entryNameLength;
        u32 valsFirstIndex;
        u16 valsCount;
        Trd::access_word accessWord;
        u32 recordChecksum = Consts::TRD_INVALID_CHKSUM;
        static constexpr int entryRecordSize() {
            return 20;
        }
        constexpr u64 size() const override {
            constexpr auto x = Impl::BinarySerializer::elementSize(
                identifier, entryNameOffset, entryNameLength, valsFirstIndex, valsCount, accessWord,
                recordChecksum);
            static_assert(x == entryRecordSize(), "Size missmatch");
            return x;
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
        void dbgInfoPrint() const override;
    };

    struct TregValueRecord : Impl::SerializableData {
        u16 identifier = Consts::Treg::VALUE_REC_ID;
        u32 valNamePoolOffset;
        u16 valNameLength;
        TregValueDatatype
            datatype; // if dataType < 11 store data in smallData otherwiese point to pool
        //with LargeData; if dataSize smaller than 8 && dataType < 11 pad with 0's until end

        //not part of the struct just definition for variant
        //were using the fields manually so no need to worry about padding
        struct PayloadPoolData {
            u32 payloadPoolOffset;
            u32 payloadSize;
        };
        Trd::access_word accessWord;
        u32 recordChecksum = Consts::TRD_INVALID_CHKSUM;
        using TrivialData = std::array<std::byte, 8>;
        // using TrivialData = std::byte[8];
        std::variant<TrivialData, PayloadPoolData> payload;

        static constexpr int valRecordSize() {
            return 23; //align everything later we need functioning prototype
        }
        constexpr u64 size() const override {
            constexpr auto x =
                Impl::BinarySerializer::elementSize(identifier, valNamePoolOffset, valNameLength,
                                                    datatype, accessWord, recordChecksum) +
                (sizeof(std::byte) * 8);
            static_assert(x == valRecordSize(), "Size missmatch");
            return x;
            //using sizeof when we are storing struct may or may not work even if packed on all platforms
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
        void dbgInfoPrint() const override;
    };

    class Value {

      public:
        Trd::Impl::TregValAccess permissions{};
        //decimal numbers are represented as a string
        using Payload = std::variant<i8, i16, i32, i64, u8, u16, u32, u64, std::string,
                                     std::vector<std::byte>>; //add later

        explicit Value(std::string_view name, TregValueDatatype dtype, Payload payload) {
            //check key name length size
            setName(name);
            type = dtype;
            //check if payload variant size is valid
            setPayload(payload);
        };

        std::string_view getName() const {
            return attributeName;
        }
        void setName(std::string_view name) {

            //check key name length size
            [[unlikely]]
            if (name.length() > Consts::Treg::KEYNAME_MAXLEN)
                throw std::runtime_error("Name exceeded max char limit");
            attributeName = name;
        }
        TregValueDatatype getValueDataType() const {
            return type;
        }
        const Payload& getPayload() const {
            return payload;
        }
        void setPayload(Payload p) {
            //check if payload variant size is valid
            std::visit(
                [&](const auto& v) {
                    if (sizeof(v) > Consts::Treg::VALUE_MAXSIZE)
                        throw std::runtime_error(
                            "Exceeded max data length limit of " +
                            std::to_string(Consts::Treg::VALUE_MAXSIZE) + "B by " +
                            std::to_string(Consts::Treg::VALUE_MAXSIZE - sizeof(v)) + "B");
                },
                this->payload);
            payload = std::move(p);
        }

        static Value dataI8(std::string n, i8 v) {
            return Value(std::move(n), TregValueDatatype::i8, v);
        }
        static Value dataU8(std::string n, u8 v) {
            return Value(std::move(n), TregValueDatatype::u8, v);
        }
        static Value dataI16(std::string n, i16 v) {
            return Value(std::move(n), TregValueDatatype::i16, v);
        }
        static Value dataU16(std::string n, u16 v) {
            return Value(std::move(n), TregValueDatatype::u16, v);
        }
        static Value dataI32(std::string n, i32 v) {
            return Value(std::move(n), TregValueDatatype::i32, v);
        }
        static Value dataU32(std::string n, u32 v) {
            return Value(std::move(n), TregValueDatatype::u32, v);
        }
        static Value dataI64(std::string n, i64 v) {
            return Value(std::move(n), TregValueDatatype::i64, v);
        }
        static Value dataU64(std::string n, u64 v) {
            return Value(std::move(n), TregValueDatatype::u64, v);
        }
        static Value dataBool(std::string n, bool v) {
            return Value(std::move(n), TregValueDatatype::u8_bool,
                         static_cast<u8>(u8bool::fromBool(v)));
        }
        static Value dataString(std::string n, std::string v) {
            return Value(std::move(n), TregValueDatatype::cstr, std::move(v));
        }
        static Value dataBinData(std::string n, std::span<std::byte> v) {
            return Value(std::move(n), TregValueDatatype::binLE,
                         std::vector<std::byte>{v.begin(), v.end()});
        }
        static Value dataSoffset(std::string n, soffset v) {
            return Value(std::move(n), TregValueDatatype::soffset, static_cast<u64>(v));
        }

        static std::optional<const char*> DatatypeAsString(TregValueDatatype vd);

        template <typename T>
        const T& get() const {
            if (!std::holds_alternative<T>(payload))
                throw std::runtime_error("Value::get<T> type missmatch");
            return std::get<T>(payload);
        }

      private:
        std::string attributeName;
        TregValueDatatype type;
        Payload payload;
    };

    class Entry {
      public:
        explicit Entry(std::string name) : name(std::move(name)) {};

        std::string_view getName() const {
            return name;
        }
        const std::vector<Value>& getValues() const {
            return values;
        }
        std::vector<Value>& getValues() {
            return values;
        }
        inline void updateValue(const Value& a) {
            IManageValue(a, true);
        }
        inline void createValue(const Value& a) {
            IManageValue(a, false);
        }

        /**
         * @brief Deletes attribute from list
         * 
         * @param name 
         * @return true if attribute was found and deleted
         * @return false if attribute does not exist
         */
        bool deleteValue(std::string_view name);

        Value const* findValue(std::string_view name) const;
        Value* findValue(std::string_view name);

        Trd::Impl::TregDirAccess permissions{};

      private:
        std::string name;
        std::vector<Value> values;

        void IManageValue(const Value& a, bool overwrite);
    };

    class Key {
      public:
        explicit Key(std::string name) : key(std::move(name)) {};
        std::string_view getName() const {
            return key;
        }
        const std::vector<Key>& getChildren() const {
            return children;
        }
        std::vector<Key>& getChildren() {
            return children;
        }

        const std::vector<Entry>& getEntries() const {
            return entries;
        }
        std::vector<Entry>& getEntries() {
            return entries;
        }

        void addChild(Key k) {
            children.push_back(std::move(k));
        }
        void addEntry(Entry v) {
            entries.push_back(std::move(v));
        }
        static void printTree(const Key& k, const std::string& prefix = "", bool isRoot = true);
        Key* findChild(std::string_view name);
        Entry* findEntry(std::string_view name);

        Trd::Impl::TregDirAccess permissions{};

      private:
        std::string key;
        std::vector<Key> children;
        std::vector<Entry> entries;
    };

    static inline bool AttrIsTrivial(TregValueDatatype type) {
        constexpr int lastTrivialAttrIndex = 127;
        if (static_cast<u8>(type) <= lastTrivialAttrIndex)
            return true;
        return false;
    };
};