/**
 * @file binarySerializer.h
 * @brief Interface for serializing and deserializing the data before writing them via TStream
 *
 *
 */
#pragma once

#include "ioflags.h"
#include <fstream>
#include <string>
#include <memory>
#include "trderr.h"
#include "tstreaminfo.h"
#include <string_view>
#include <filesystem>
#include <algorithm>
#include <vector>
#include "boost/crc.hpp"
// todo add most basic IO function here
// internal functions used by TStream
namespace Trd::Impl {
    /* no use for now
     class Descriptor {
         public:
             virtual ~Descriptor() = default;
             //beg=true mean beginning of section, beg=false end of section
             virtual std::expected<void, Err::TrdError> writeDescriptorSUID() = 0;
             virtual std::expected<void, Err::TrdError> readDescriptorSUID() = 0;
             virtual bool  isValidSUID() = 0;

     };
     */
    template <typename T>
    concept ConTriviablyCopyable = std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;

    /**
     * @brief serializes struct to array of bytes.
     * @details The process of passing/retrieving data should use Impl::SerializableData interface
     */
    class BinarySerializer {
      private:
        std::vector<std::byte> bufferData{};
        std::endian emulEndianness{std::endian::native};
        soffset readOffset = 0;
        bool requireAlignment = true;

        template <ConTriviablyCopyable T>
        // returns number of bytes read
        u64 iReadTrivialEx(soffset offset, T& t) {
            // removed assertion because we might pass enum
            // static_assert(std::is_fundamental_v<std::remove_pointer_t<T>>, "Only fundamental types are supported.");
            // nulllptr handlerd here
            const u64 readSize = readRaw(&t, sizeof(t), offset);
            if (!isInstanceLittleEndian()) {
                t = reverseByteOrder(t);
            }
            return readSize;
        }
        template <ConTriviablyCopyable T>
        void iAddContainer(const std::span<const T> arr) {
            if (arr.empty()) {
                throw std::invalid_argument("Array is empty");
            }

            for (const auto& v : arr) {
                addTrivial(v);
            }
        }
        template <ConTriviablyCopyable T>
        void iAddTrivial(T t) {
            // removed assertion because we might pass enum
            // static_assert(std::is_fundamental_v<T>, "Only fundamental types are supported.");
            if (!isInstanceLittleEndian()) {
                t = reverseByteOrder(t);

                // t = std::byteswap(t);
            }
            addRaw(&t, sizeof(t));
        }
        //  std::weak_ptr<Trd::Tstream::TStreamInfo> wFstr;
      public:
        /// compile time constant way of checking the host endianness
        static bool isLittleEndian() noexcept;
        /// dynamically checks the host endianness
        bool isInstanceLittleEndian() const noexcept;

        BinarySerializer(std::endian emulated = std::endian::native, bool requireAlignment = true) :
            emulEndianness(emulated), requireAlignment(requireAlignment) {};

        // second constructor for deserialize()
        BinarySerializer(std::vector<std::byte> data, std::endian emulated = std::endian::native,
                         soffset xOffset = 0, bool requireAlignment = true) :
            bufferData(std::move(data)), emulEndianness(emulated), readOffset(xOffset),
            requireAlignment(requireAlignment) {};

        template <ConTriviablyCopyable T>
        // reverses byte order of variable
        // we are intentionally passing by value as we modify the original variable
        // the bitcast creates copy as well
        static constexpr T reverseByteOrder(T var) {
            if constexpr (std::is_integral_v<T>) {
                return std::byteswap(var);
            } else {
                auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(var);
                std::reverse(bytes.begin(), bytes.end());
                return std::bit_cast<T>(bytes);
            }
        }
        template <ConTriviablyCopyable T>
        static constexpr void reverseByteOrderInPlace(T& var) {
            if constexpr (std::is_integral_v<T>) {
                var = std::byteswap(var);
            } else {
                auto* bytes = reinterpret_cast<std::byte*>(&var);
                std::reverse(bytes, bytes + sizeof(T));
            }
        }

        // make the datatype 8 byte aligned
        static u64 getByteAlignment(u64 varSize);
        /*
        /if data size is no aligned the function manipulating the data needs to fix it
        by calling GetByteAlignment()
        Why wont we use this always instead of killing on failure ? Well if malformed data enters
        the function we could possibly write bad data
        */

        /**
         * @brief Determines if the size of the data is properly aligned
         *
         * @param size byte size of data to check
         * @return true
         * @return false
         */
        static constexpr bool isDataSizeAligned(u64 size) {
            return size % Trd::Consts::Binary::BSERIALIZE_DATA_ALIGN == 0;
        }

        /**
         * @brief Throws std::runtime_error if data is misaligned
         *
         * @param size size of data in bytes
         * @return true
         * @return false
         */
        static bool expectAlignedDataOrDie(u64 size);
        soffset getReadOffset() const {
            return readOffset;
        }
        void setReadOffset(soffset offset) {
            if (offset != UINT64_MAX)
                readOffset = offset;
        }
        void resetReadOffset() {
            setReadOffset(0);
        }

        const std::vector<std::byte>& getData() const {
            return bufferData;
        }
        std::vector<std::byte> getData() {
            return bufferData;
        }
        u64 getDataSize() const {
            return (getData().size() * sizeof(std::byte));
        }
#ifdef _LIBTRIDENT_DEBUG
        TRD_DBG_BUILD_ONLY void dbgDumpData() const;
#endif
        /// clears the stored data
        inline void emptyData() {
            bufferData.clear();
        }
        /**
         * @brief Serializes raw data to array. Caller is responsible for handling the endianness.
         * @warning UNSAFE API Do not use this unless you know what you are doing and absolutely need to.
         * This function doesn't do any endianness checking nor handling. Do not pass data with improper
         * endianness otherwise the written data will get corrupted
         *
         * @param data non null and valid pointer to the block of data to add
         * @param size size of the data
         */
        TRD_UNSAFE_API void addRaw(const void* data, u64 size);

        /**
         * @brief Serializes data containers and STL types passed as std::span
         * @param arr data to be serialized
         */
        template <typename... Ts>
        void addContainer(const Ts&... args) {
            (iAddContainer(args), ...);
        }
        /**
         * @brief
         *
         * @param offset offset which to start reading from. This field has to be correct
         * and is obtained by adding the return of previous readXXXX function call
         * @param t variable which receives the data
         * @return u64 offset to increase the readSize by
         */
        template <typename... Ts>
        u64 readTrivialEx(soffset offset, Ts&... args) {
            soffset zOffset = offset;
            ((zOffset += iReadTrivialEx(zOffset, args)), ...);
            // we have to substract from the original offset otherwise we may get misleading results
            // when offset is nonzero
            return zOffset - offset;
        }
        template <typename... Ts>
        void readTrivial(Ts&... args) {
            ((readOffset += readTrivialEx(readOffset, args)), ...);
        }

        /**
         * @brief Reads to STL containers and data passed as std::span
         *
         * @tparam T trivially copyable object
         * @param arr
         * @param offset offset where to start reading from
         * @return u64 offset to increase the readSize by
         */
        template <ConTriviablyCopyable T>
        u64 readContainerEx(std::span<T> arr, soffset offset = 0) {

            if (arr.empty()) {
                throw std::invalid_argument("Array is empty");
            }
            u64 bytesRead = 0;
            soffset bytePos = offset;

            for (u64 i = 0; i < arr.size(); ++i) {
                const u64 readSize = readTrivialEx(bytePos, arr[i]);
                bytePos += readSize;
                bytesRead += readSize;
            }
            return bytesRead;
        }
        template <ConTriviablyCopyable T>
        void readContainer(std::span<T> arr) {
            readOffset += readContainerEx(std::move(arr), readOffset);
        }

        // Warning this method DOES NOT check nor modify endianness
        // Do not use this method unless no other override is available
        // When calling this function you are responsible for passing data in correct endianness
        // throws std::invalid_argument on failure
        /**
         * @brief reads raw data without handling endianness or data type. Do not use unless you have to.
         * @details
         * @param dataOut non null pointer to valid data which receives th read information. Buffer has to be
         * big enough to hold all data otherwise we are overflowing
         * @param size size of data in bytes
         * @param offset offset where to start reading from
         * @warning Do not use this unless you know what you are doing and absolutely need to.
         * This function doesn't do any endianness checking nor handling. Do not pass data with improper
         * endianness otherwise the written data will get corrupted
         */
        TRD_UNSAFE_API u64 readRaw(void* dataOut, u64 size, soffset offset = 0);

        constexpr static u64 elementSize() {
            return 0;
        }
        /**
         * @brief Simple way to get size of multiple elements
         * @warning For complex data structures without sizeof override you cannot use this function
         * @param args list of variables
         * @return constexpr u64 sum of all elements
         */
        template <typename T, typename... Ts>
        constexpr static u64 elementSize(const T&, const Ts&... args) {
            return sizeof(T) + elementSize(args...);
        }

        /**
         * @brief Adds data for serialization
         *
         * @tparam T trivially copyable object
         * @param t data to be serialized
         */
        template <typename... Ts>
        void addTrivial(const Ts&... args) {
            (iAddTrivial(args), ...);
        }
        /*
        if autoalign is set then we align all bytes to the Consts::Binary::BSERIALIZE_DATA_ALIGN
          bool autoalign aligns elements to the correct size but this should not be used because
          when receiving the data you now have to explicitly move the offset at the element,
          this is confusing to anyone reading the function, instead if you have information that
          doesnt match the data add _reserved or _padding field
        */

        /**
         * @brief Retrieves the serialized data
         *
         * @param autoAlign false by default. This option generally should not be set to true
         * as this could mees up the offsets
         * @return std::optional<std::vector<std::byte>> the serialized data if present
         */
        std::optional<std::vector<std::byte>> getFormattedData(bool autoAlign = false);

        /**
         * @brief Same as writeAlignedDataToTStream but requires the data to be aligned
         * 
         * @param tStream 
         * @param data 
         * @param requireAlignment throws std::invalid_argument if data is misaligned
         * @param seekPos 
         * @param seekDir 
         */
        static void writeDataToTStream(Trd::Impl::TStreamInfo& tStream,
                                       std::span<const std::byte> data, bool requireAlignment,
                                       soffset seekPos = 0,
                                       std::ios_base::seekdir seekDir = std::ios::beg,
                                       bool keepOriginalSeek = true);

        /**
         * @brief Reads data from TStream as vector with unserialized data.
         * @param tStream tStream instance
         * @param seekPos fseek position where to sttart writing
         * @param size size of the data to read
         * @param requireAlignment throws std::invalid_argument if data is misaligned
         * @return std::vector<std::byte> unserialized raw data from file
         */
        static std::vector<std::byte> readDataFromTStream(Trd::Impl::TStreamInfo& tStream,
                                                          soffset seekPos, u64 size,
                                                          bool requireAlignment,
                                                          bool keepOriginalSeek = true);
    };
    /**
     * @brief Generates CRC32 and handles endianness for multiple types and containers
     *
     */
    class Crc32Gen {
      private:
        boost::crc_32_type crc{};

      public:
        /// retrieves the calculated CRC32 value
        /// @return
        u32 getCrc32() const {
            return crc.checksum();
        }
        boost::crc_32_type getCrcProviderObj() const {
            return crc;
        }
        /// clears the internal buffer
        void reset() {
            crc.reset();
        }

        template <typename... Ts>
        void addData(const Ts&... args) {
            (addData(args), ...);
        }

        /// override for trivial data types
        template <ConTriviablyCopyable T>
        void addData(T v) {
            addData(std::span<const T>{&v, 1});
        }

        /// addData override for std::vector
        template <ConTriviablyCopyable T>
        void addData(const std::vector<T>& v) {
            addData(std::span<const T>{v});
        }
        /// addData override for std::array
        template <ConTriviablyCopyable T, u64 N>
        void addData(const std::array<T, N>& arr) {
            addData(std::span<const T>{arr});
        }
        /// addData override for C styled array
        template <ConTriviablyCopyable T, u64 N>
        void addData(const T (&arr)[N]) {
            addData(std::span<const T>{arr});
        }
        /**
         * @brief Adds data for CRC32 calculation. Generally you should use provided override
         * @param bytes  span of data
         */
        template <ConTriviablyCopyable T>
        void addData(const std::span<const T> bytes) {
            // BE, reverse  byte order
            if (!BinarySerializer::isLittleEndian()) {
                std::vector<T> data(bytes.begin(), bytes.end());
                // resize instead of reserve, this is not mistake
                // because begin iterator wont work otherwise
                // data.resize(bytes.size());
                //   std::copy(bytes.begin(), bytes.end(), data.begin());
                for (auto& x : data) {
                    BinarySerializer::reverseByteOrderInPlace(x);
                }
                crc.process_bytes(data.data(), data.size() * sizeof(T));
                // crc = ::crc32(crc, reinterpret_cast<const Bytef*>(data.data()),
                //              static_cast<u32>(data.size() * sizeof(T)));
            } else {
                crc.process_bytes(bytes.data(), bytes.size_bytes());
                // crc = ::crc32(crc, reinterpret_cast<const Bytef*>(bytes.data()),
                //               static_cast<u32>(bytes.size_bytes()));
            }
        }
    };
};
