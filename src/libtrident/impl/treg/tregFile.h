#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"
#include <variant>
namespace Trd::Consts::Treg {
    inline constexpr u32 TREG_MAGIC = 0x52444852; //RHDR (BE)
    inline constexpr u16 KEY_REC_ID = 0x526B; //kR (BE)
    inline constexpr u16 VAL_REC_ID = 0x5276; //vR (BE)
    inline constexpr u16 ATTR_REC_ID = 0x5261; //aR (BE)
};

namespace Trd {
    namespace RegFlags {
        enum Flag : u16 {
            Clear = 0
        };
    };
    enum class TregAttrDatatype : u8 {
        i8 = 0,
        i16 = 1,
        i32 = 2,
        i64 = 3,
        u8 = 4,
        u16 = 5,
        u32 = 6,
        u64 = 7,
        f32 = 8,
        f64 = 9,
        u8_bool = 10,
        cstr = 11, // null terminated
        binDataLE = 12,
        //   binDataBE = 13 //not implemented currently
    };

    struct TregHeader : Impl::SerializableData {
        /*
        u32 magic{Consts::Treg::TREG_MAGIC};
        RegFlags::Flag flags{RegFlags::Clear};
        file_offset regRootOffset;
        u64 regSizeTotal;
        u64 _reserved;
        u16 _reserved1;
*/

        u32 magic{Consts::Treg::TREG_MAGIC};
        RegFlags::Flag flags{RegFlags::Clear};
        u32 keyEntriesCount;
        u32 valEntriesCount;
        u32 attrEntriesCount;
        u64 poolSize;
        u32 checksum; //everything after header until the end of treg section
        u16 _reserved1 = 0;

        static constexpr int headerSize() {
            return 32;
        }

        constexpr u64 size() const override {
            constexpr auto x = Impl::BinarySerializer::elementSize(
                magic, flags, keyEntriesCount, valEntriesCount, attrEntriesCount, poolSize,
                checksum, _reserved1);
            static_assert(x == headerSize(), "Size missmatch");
            return x;
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
        //doesnt make sense here, only in final TregObject
        //  std::optional<Impl::Crc32Gen> checksumCRC32() const;
    };
    //singular key record
    struct TregKeyRecord : Impl::SerializableData {
        u16 identifier = Consts::Treg::KEY_REC_ID; // just a mark to fill instead of padding
        u32 keyNameOffset; //offset in flat vector
        u16 keyNameLength;
        u32 firstChildKeyIndex;
        u16 childKeysCount;

        u32 firstValIndex;
        u16 valueCount;
        u32 recordChecksum = Consts::TRD_INVALID_CHKSUM;
        /*
        sum of all fields in bytes, there is probably a way better way to do this but i want this 
        to be static and i dont want to use magic numbers that make no sense
        */
        static constexpr int keyRecordSize() {
            return 24;
        }
        constexpr u64 size() const override {
            constexpr auto x = Impl::BinarySerializer::elementSize(
                identifier, keyNameOffset, keyNameLength, firstChildKeyIndex, childKeysCount,
                firstValIndex, valueCount, recordChecksum);

            static_assert(x == keyRecordSize(), "Size missmatch");
            return x;
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
    };

    struct TregValueRecord : Impl::SerializableData {
        u16 identifier = Consts::Treg::VAL_REC_ID;
        u32 valNameOffset;
        u16 valNameLength;
        u32 attrFirstIndex;
        u16 attrCount;
        u32 recordChecksum = Consts::TRD_INVALID_CHKSUM;
        static constexpr int valueRecordSize() {
            return 18;
        }
        constexpr u64 size() const override {
            constexpr auto x =
                Impl::BinarySerializer::elementSize(identifier, valNameOffset, valNameLength,
                                                    attrFirstIndex, attrCount, recordChecksum);
            static_assert(x == valueRecordSize(), "Size missmatch");
            return x;
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
    };
    struct TregAttrRecord : Impl::SerializableData {
        u16 identifier = Consts::Treg::ATTR_REC_ID;
        u32 attrNamePoolOffset;
        u16 attrNameLength;
        TregAttrDatatype
            datatype; // if dataType < 11 store data in smallData otherwiese point to pool
        //with LargeData; if dataSize smaller than 8 && dataType < 11 pad with 0's until end

        //not part of the struct just definition for variant
        struct PayloadPoolData {
            u32 payloadPoolOffset;
            u32 payloadSize;
        };
        //
        u32 recordChecksum = Consts::TRD_INVALID_CHKSUM;
        using TrivialData = std::array<std::byte, 8>;
        // using TrivialData = std::byte[8];
        std::variant<TrivialData, PayloadPoolData> payload;

        static constexpr int attrRecordSize() {
            return 21; //align everything later we need functioning prototype
        }
        constexpr u64 size() const override {
            constexpr auto x =
                Impl::BinarySerializer::elementSize(identifier, attrNamePoolOffset, attrNameLength,
                                                    datatype, recordChecksum) +
                (sizeof(std::byte) * 8);
            static_assert(x == attrRecordSize(), "Size missmatch");
            return x;
            //using sizeof when we are storing struct may or may not work even if packed on all platforms
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
    };

    class Attr {

        /*
        i8 = 0,
        i16 = 1,
        i32 = 2,
        i64 = 3,
        u8 = 4,
        u16 = 5,
        u32 = 6,
        u64 = 7,
        f32 = 8,
        f64 = 9,
        u8_bool = 10,
        cstr = 11, // null terminated
        binDataLE = 12,
       */
      public:
        using Payload = std::variant<i8, i16, i32, i64, u8, u16, u32, u64, float, double, bool,
                                     std::string, std::vector<std::byte>>; //add later

        explicit Attr(std::string name, TregAttrDatatype dtype, Payload payload) :
            attributeName(std::move(name)), type(dtype), payload(std::move(payload)) {};

        std::string_view getName() const {
            return attributeName;
        }
        TregAttrDatatype getAttrDataType() const {
            return type;
        }
        const Payload& getPayload() const {
            return payload;
        }
        void setPayload(Payload p) {
            payload = std::move(p);
        }

        static Attr dataU8(std::string n, u8 v) {
            return fillTrivial(std::move(n), TregAttrDatatype::u8, v);
        }
        static Attr dataU16(std::string n, u16 v) {
            return fillTrivial(std::move(n), TregAttrDatatype::u16, v);
        }
        static Attr dataU32(std::string n, u32 v) {
            return fillTrivial(std::move(n), TregAttrDatatype::u32, v);
        }
        static Attr dataU64(std::string n, u64 v) {
            return fillTrivial(std::move(n), TregAttrDatatype::u64, v);
        }
        //fill later
        template <typename T>
        static Attr fillTrivial(std::string name, TregAttrDatatype type, T value,
                                std::endian forceByteOrder = std::endian::native) {

            Impl::BinarySerializer bs(forceByteOrder, false);
            bs.addTrivial(value);
            auto fmt = bs.getFormattedData();
            if (!fmt)
                throw std::runtime_error("Attribute data could not be properly serialized");

            return Attr(std::move(name), type, std::move(fmt.value()));
        }

        template <typename T>
        const T& get() const {
            if (!std::holds_alternative<T>(payload))
                throw std::runtime_error("Attr::get<T> type missmatch");
            return std::get<T>(payload);
        }

      private:
        std::string attributeName;
        TregAttrDatatype type;
        Payload payload;
    };

    class Value {
      public:
        explicit Value(std::string name) : name(std::move(name)) {};

        std::string_view getName() const {
            return name;
        }
        const std::vector<Attr>& getAttrs() const {
            return attrs;
        }
        std::vector<Attr>& getAttrs() {
            return attrs;
        }
        void addAttr(Attr a) {
            attrs.push_back(std::move(a));
        }
        Attr const* findAttr(std::string_view name) const {
            for (const auto& x : attrs) {
                if (x.getName() == name)
                    return &x;
            }
            return nullptr;
        }
        Attr* findAttr(std::string_view name) {
            for (auto& x : attrs) {
                if (x.getName() == name)
                    return &x;
            }
            return nullptr;
        }

      private:
        std::string name;
        std::vector<Attr> attrs;
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

        const std::vector<Value>& getValues() const {
            return values;
        }
        std::vector<Value>& getValues() {
            return values;
        }

        void addChild(Key k) {
            children.push_back(std::move(k));
        }
        void addValue(Value v) {
            values.push_back(std::move(v));
        }
        Key* findChild(std::string_view name) {
            for (auto& x : children) {
                if (x.getName() == name)
                    return &x;
            }
            return nullptr;
        }
        Value* findValue(std::string_view name) {
            for (auto& x : values) {
                if (x.getName() == name)
                    return &x;
            }
            return nullptr;
        }

      private:
        std::string key;
        std::vector<Key> children;
        std::vector<Value> values;
    };

    static inline bool AttrIsTrivial(TregAttrDatatype type) {
        constexpr int lastTrivialAttrIndex = 10;
        if (static_cast<u8>(type) <= lastTrivialAttrIndex)
            return true;
        return false;
    };

};