#pragma once
#include <stdint.h>
#include <stddef.h>
#include <optional>
#include <vector>
#include "datatypes.h"
namespace LibTrident::Impl {
    //struct data to be serialized using BinarySerializer class
    //BinarySerializer as well as Impl::Crc32Gen handle endianness correctly 
    //no need to bitswap or do any data manipulations
    struct SerializableData{
        //size returned by elementSize()
        virtual constexpr size_t size() const = 0;

        virtual ~SerializableData() = default;
        //serialized data returned by getFormattedData()
        virtual std::optional<std::vector<u8>> serialize() const = 0;
        //data read and manually deserialized
        virtual bool deserialize(const std::vector<u8>& dataIn) = 0;
        //optional, data checksumed using Impl::Crc32Gen
        virtual std::optional<u32> checksumCRC32() const {
            return std::nullopt;
        }

    };
}