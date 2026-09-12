#pragma once

#include "datatypes.h"
#include <optional>
#include "trderr.h"
#include "dynamicTable.h"
#include "tregFile.h"
#include "binarySerializer.h"
#include <ranges>
#include <cstring>
/*
this will be the most complex part of the whole format and WILL be rewritten multiple times
because it will be bug infested mess if we want things as in place operations and such in future

TREG will be a metadata storage (key-value) for each dtbl directory entry
featuring simplistic filesystem like directories/entries, modifiable attributes,
dynamic fields and table specific information that will be embedded most likely as XATTR attribute
byte array that will be handled by each TABLE differently if needed

the binary data layout will look something like this 

[treg header] [key section] [value section] [attribute section] [data pool]
to have aligned offsets properly i decided to store the large variable strings/ binary data
and in future to avoid repetition strings all of the data instead of being duplicit is stored here

now we can dynamically calculate where each section inside treg starts and ends
    start_key = sizeof(rhdr)
    start_value = start_key + (no_key_entries * sizeof(key_entry))
    start_attr = start_value + (no_val_entries * sizeof(val_entry))
    pool_start = start_attr + (no_attr_entries * sizeof(attr_entry)) 



if parser doesnt know what is some certain table doing they can just currentOffset += cellSize

The first version will probably be immutable meaning that to rewrite or update the data we have to
firsly read the data then modify in memory then write at offset if we are adding fields we need
to do COW the entire treg. In future i definitely want windows like registry where data can be added/deleted
without copying to the new file and deleting certain tables, finding holes (cellsize negative if free like some fs do it)

also we need an interface for CELLDATA tables so the tables are required to fill the basic info

each cell should contain header crc ?

Key - main key record may contain subkeys
Entry - junction M:N table between keys and valus
Value - contains stored data, its datatype and other info
*/

/*
TODO add string and bytes support, restrict key names to [A-Za-z0-9_-]
fill in record checksums they are invalid
*/
namespace Trd {
    class TrPkg;

    //todo namespace instead ??
    class Treg {
      public:
        //high level interface
        //this is ugly

        explicit Treg(TrPkg& pkg) : trpkg(pkg) {};

      private:
        friend class TrPkg;
        TrPkg& trpkg;
    };
    class TregHiveSerializer {
      private:
        //  std::vector<TregKeyRecord> keys;
        //   std::vector<TregValueRecord> values;
        //  std::vector<TregAttrRecord> attrs;
        //  std::vector<std::byte> dataPool;
        using PoolData = std::vector<std::byte>;
        struct KeyMapPool;
        static std::vector<KeyMapPool> iRecordKeys(const Key& root);
        //helper, adds data to the datapool and returns the offset to the name and its length
        static std::pair<u32, u16> iPoolAppendName(PoolData& p, std::string_view name);
        //helper for std::visit
        static u32 iPoolAppendBytes(PoolData& p, std::span<const std::byte> bytes);

        template <typename T>
        static std::vector<const T*> iSortByName(const std::vector<T>& t) {
            std::vector<const T*> sorted;
            sorted.reserve(t.size());
            for (const T& x : t) {
                sorted.push_back(&x);
            }
            std::ranges::sort(sorted, {}, [](const T* p) { return p->getName(); });
            return sorted;
        }
        template <typename T>
        static void serializeEntity(const T& t, std::vector<std::byte>& out) {
            for (const auto& x : t) {
                const auto serialized = x.serialize();
                if (!serialized)
                    throw std::runtime_error("failed to serialize entity");
                //    x.dbgInfoPrint();
                out.append_range(std::move(serialized.value()));
            }
        }

        static TregValueRecord iRecordValues(const Value& attr, PoolData& pool);
        static TregEntryRecord iRecordEntries(const Entry& val, PoolData& pool,
                                              std::vector<TregValueRecord>& attrRec);

        static std::vector<std::byte> serializeData(const std::vector<TregKeyRecord>& keys,
                                                    const std::vector<TregEntryRecord>& val,
                                                    const std::vector<TregValueRecord>& attr,
                                                    const PoolData& pool);

      public:
        static std::expected<std::vector<std::byte>, Trd::Err::TrdError>
        build(const Trd::Key& root);
    };
};
