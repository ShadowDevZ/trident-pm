/**
 * @file serdatacommon.h
 * @brief Interface for serializing/deserializing and handling data written to TStream
 * 
 * 
 */
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <optional>
#include <vector>
#include "datatypes.h"
namespace LibTrident::Impl {
    
    /**
     * @brief struct data to be serialized using BinarySerializer class
     * @details BinarySerializer as well as Impl::Crc32Gen handle endianness correctly 
     * no need to bitswap or do any data manipulations
     */
    struct SerializableData{
        /// size of all elements ideally returned by elementSize()
        virtual constexpr size_t size() const = 0;

        virtual ~SerializableData() = default;
        /**
         * @brief Function to be called to retrieve the serialized data
         * 
         * @return std::optional<std::vector<u8>> serialized data returned by getFormattedData()
         */
        virtual std::optional<std::vector<u8>> serialize() const = 0;
        /**
         * @brief Function to be called which manually deserializes and assigns the data
         * 
         * @param[in] dataIn serialized data passed by caller 
         * @return true if serialization succeeded
         * @return false if serialization failed, caller is reponsible for handling the error
         */
        virtual bool deserialize(const std::vector<u8>& dataIn) = 0;
        /**
         * @brief optional, data checksumed using Impl::Crc32Gen
         * 
         * @return std::optional<u32> crc32 chechksum, if available
         */
        virtual std::optional<u32> checksumCRC32() const {
            return std::nullopt;
        }

    };
}