#pragma once

#include "datatypes.h"
#include <optional>
#include "trderr.h"
#include "dynamicTable.h"
#include "tregDefs.h"
#include "binarySerializer.h"
#include <ranges>
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

cell data info varies depending whether its K,V, or A, they share in common the CELL_INFO struct
each entry also has internal CRC32 validation. Also the cell data contains attribute on child nodes.

The first version will probably be immutable meaning that to rewrite or update the data we have to
firsly read the data then modify in memory then write at offset if we are adding fields we need
to do COW the entire treg. In future i definitely want windows like registry where data can be added/deleted
without copying to the new file and deleting certain tables, finding holes (cellsize negative if free like some fs do it)

also we need an interface for CELLDATA tables so the tables are required to fill the basic info

each cell should contain header crc ?
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
        struct KeyMapPool {
            const Trd::Key* key;
            u32 childStart{0};
            u32 childCount{0};
        };
        static std::vector<KeyMapPool> iGetKeyPoolMap(const Key& root) {
            std::vector<KeyMapPool> plan;
            plan.push_back({&root, 0, 0});
            for (size_t i = 0; i < plan.size(); ++i) {
                std::vector<const Trd::Key*> sorted = iSortByName(plan[i].key->getChildren());
                plan[i].childStart = static_cast<u32>(plan.size());
                plan[i].childCount = static_cast<u32>(sorted.size());
                for (const Key* k : sorted) {
                    plan.push_back({k, 0, 0});
                }
            }
            return plan;
        }
        //helper, adds data to the datapool and returns the offset to the name and its length
        static std::pair<u32, u16> iPoolAppendName(PoolData& p, std::string_view name) {
            u32 offset = static_cast<u32>(p.size());
            const auto* data = reinterpret_cast<const std::byte*>(name.data());
            p.insert(p.end(), data, data + name.size());
            return {offset, static_cast<u16>(name.size())};
        }
        static u32 iPoolAppendBytes(PoolData& p, std::span<const std::byte> bytes) {
            u32 offset = static_cast<u32>(p.size());
            p.insert(p.end(), reinterpret_cast<const std::byte*>(bytes.data()),
                     reinterpret_cast<const std::byte*>(bytes.data() + bytes.size_bytes()));
            return offset;
        }

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

        bool iRecordKeys();
        bool iRecordValues();
        bool iRecordAttributes();

      public:
        std::expected<std::vector<std::byte>, Trd::Err::TrdError> build(const Trd::Key& root);
    };
};