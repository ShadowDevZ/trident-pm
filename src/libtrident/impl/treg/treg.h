#pragma once

#include "datatypes.h"
#include <optional>
#include "trderr.h"
#include "dynamicTable.h"
#include "tregDefs.h"
#include "binarySerializer.h"
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

    class Treg {
        //high level interface
        //this is ugly
        struct Attr {
            std::string attributeName;
            TregAttrDatatype type;
            std::vector<std::byte> data;

            static TregAttrDatatype u8(std::string n, u8 v) {
                return fillTrivial(std::move(n), TregAttrDatatype::u8, v);
            }
            static TregAttrDatatype u16(std::string n, u16 v) {
                return fillTrivial(std::move(n), TregAttrDatatype::u16, v);
            }
            static TregAttrDatatype u32(std::string n, u32 v) {
                return fillTrivial(std::move(n), TregAttrDatatype::u32, v);
            }
            static TregAttrDatatype u64(std::string n, u64 v) {
                return fillTrivial(std::move(n), TregAttrDatatype::u64, v);
            }
            //fill later
            template <typename T>
            static TregAttrDatatype fillTrivial(std::string name, TregAttrDatatype type, T value) {

                attr.attributeName = std::move(name);
                attr.type = type;

                Impl::BinarySerializer bs;
                Attr attr;
                bs.addTrivial(value);
                auto fmt = bs.getFormattedData();
                if (!fmt)
                    throw std::runtime_error("Attribute data could not be properly serialized");
                attr.data = fmt.value();

                return attr;
            }
        };

        struct Value {
            std::string value;
            std::vector<Attr> attrs;
        };

        struct Key {
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

        explicit Treg(TrPkg& pkg) : trpkg(pkg) {};

      private:
        friend class TrPkg;
        TrPkg& trpkg;
    };
};