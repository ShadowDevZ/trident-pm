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

//todo add most basic IO function here
//internal functions used by TStream
namespace LibTrident::PkgIO {
   
    class Descriptor {
        public:
            virtual ~Descriptor() = default;
            //beg=true mean beginning of section, beg=false end of section
            virtual std::expected<void, Err::TrdError> writeDescriptorSUID() = 0;
            virtual std::expected<void, Err::TrdError> readDescriptorSUID() = 0;
            virtual bool  isValidSUID() = 0;
           
    };
    template <typename T>
    concept ConTriviablyCopyable = std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;


    //serializes struct to array of bytes
    //each functiopn which wants to utilize TStream and use non trivial datatypes must implement Serialize() function
    class BinarySerializer {
private:
    std::vector<uint8_t> bufferData {};
    std::endian emulEndianness{std::endian::native};
  //  std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr;
public:
        //checks if the whole project is little endian
        //can only be overriden at compile time for debugging
        static bool isLittleEndian() {
            #if LT_DEBUG_ENDIAN_FORCE == 1
                return true;
            #elif LT_DEBUG_ENDIAN_FORCE == 2
                return false;
            #else
                return std::endian::native == std::endian::little;
            #endif
        }
        //checks if the current BinarySerializer class is little endian
        //can be overriden through constructor
        bool isInstanceLittleEndian() {
            #if LT_DEBUG_ENDIAN_FORCE == 1
                return true;
            #elif LT_DEBUG_ENDIAN_FORCE == 2
                return false;
            #else
                return emulEndianness == std::endian::little;
            #endif
        }
  
    BinarySerializer(std::endian emulated = std::endian::native) : emulEndianness(emulated) {}
    //second constructor for deserialize()
    BinarySerializer(const std::vector<uint8_t>& data, std::endian emulated = std::endian::native
    ) : bufferData(data), emulEndianness(emulated) {}
    
    BinarySerializer(const BinarySerializer& other) : bufferData(other.bufferData),
    emulEndianness(other.emulEndianness) {}

    BinarySerializer(BinarySerializer&& other) : bufferData(other.bufferData),
    emulEndianness(other.emulEndianness) {}
            
    
    template <ConTriviablyCopyable T>
    constexpr T reverseByteOrder(T var) {
        if constexpr (std::is_integral_v<T>) {
            return std::byteswap(var);
        }
        else {
            auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(var);
            std::reverse(bytes.begin(), bytes.end());
            return std::bit_cast<T>(bytes);
        }
     }
     
     
    /*
    template <typename T>
    static T ReverseByteOrder(T var) {
        static_assert(std::is_trivially_copyable_v<T>,
            "Cannot reverse non-trivial type. For non-trivial types, implement serialize().");

        T reversed;

        const unsigned char* src = reinterpret_cast<const unsigned char*>(&var);
        unsigned char* dst = reinterpret_cast<unsigned char*>(&reversed);
        
        std::reverse_copy(src, src + sizeof(T), dst);
        return reversed;

    }
    */
        
        //make the datatype 8 byte aligned
            
    static size_t getByteAlignment(size_t varSize) {
              
        constexpr auto alignBytes = Consts::Binary::BSERIALIZE_DATA_ALIGN;
                
        if (varSize % alignBytes) {
            varSize += (alignBytes - (varSize % alignBytes));
        }

        return varSize;
    }
    /*
    /if data size is no aligned the function manipulating the data needs to fix it
    by calling GetByteAlignment()
    Why wont we use this always instead of killing on failure ? Well if malformed data enters 
    the function we could possibly write bad data
    */
    static constexpr bool isDataSizeAligned(size_t size) {
        return size % LibTrident::Consts::Binary::BSERIALIZE_DATA_ALIGN == 0;
    }

    //If data is ok returns true otherwise false
    //same as IsDataSizeAligned except that this leaves no room for fixing the size by padding it
    //kills program if size is not properly aligned
    static inline bool expectAlignedDataOrDie(size_t size) {
        //normal assert used because this condition simply cant happen
        bool aligned = isDataSizeAligned(size);
        if (!aligned) {
            throw std::runtime_error("Passed data was not properly aligned");
        }
        //just in case the assertion fails
        return aligned;

    } 
    
    const std::vector<uint8_t>& getData() const {
        return bufferData;
    }
    std::vector<uint8_t>& getData() {
        return bufferData;
    }
    inline void emptyData() {
        bufferData.clear();
    }
    //Warning this method DOES NOT check nor modify endianness
    //Do not use this method unless no other override is available
    //When calling this function you are responsible for passing data in correct endianness
    //throws std::invalid_argument on failure
    LT_UNSAFE_API void addRaw(const void*  data, size_t size);
    
   
    //C styled array override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <ConTriviablyCopyable T, std::size_t N>
    void addType(const T (&arr)[N]) {
        if (N < 1) {
            throw std::invalid_argument("Array is empty");
        }

        for (std::size_t i = 0; i < N; ++i) {
            addTrivial(arr[i]);
        }
    }
    //std::array override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <ConTriviablyCopyable T, std::size_t N>
    void addType(const std::array<T, N>& arr) {
        if (arr.empty()) {
            throw std::invalid_argument("Array is empty");
        }

        for (const auto& v : arr) {
            addTrivial(v);
        }
       
    }
    //std::vector override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <ConTriviablyCopyable T>
    void addType(const std::vector<T>& vec) {
        if (vec.empty()) {
            throw std::invalid_argument("Vector is empty");
        }

        for (const auto& v : vec) {
            addTrivial(v);
        }
    }

    //todo
    //reads sizeof T from bufferData and writes to ptrOut
    
    template <ConTriviablyCopyable T>
    //returns number of bytes read
    size_t readTrivial(T* t, size_t offset=0) {
        static_assert(std::is_fundamental_v<std::remove_pointer_t<T>>, "Only fundamental types are supported.");
        //nulllptr handlerd here
        size_t readSize = readRaw(t, sizeof(*t), offset);
        if (!isInstanceLittleEndian()) {
            *t = reverseByteOrder(*t);
           // *t = std::byteswap(*t);
        }
        return readSize;
    }


    template <ConTriviablyCopyable T>
    size_t readType(std::span<T> arr, size_t offset=0) {
   
        if (arr.empty()) {
            throw std::invalid_argument("Array is empty");
        }
        size_t bytesRead = 0;
        size_t bytePos = offset;
        
        for (std::size_t i = 0; i < arr.size(); ++i) {
            size_t readSize = readTrivial(&arr[i], bytePos);
            bytePos += readSize;
            bytesRead += readSize;
            dbgprintf("dd:%X\n", arr[i]);
            
        }
        return bytesRead;
    }
    
    //Warning this method DOES NOT check nor modify endianness
    //Do not use this method unless no other override is available
    //When calling this function you are responsible for passing data in correct endianness
    //throws std::invalid_argument on failure
    LT_UNSAFE_API size_t readRaw(void*  dataOut, size_t size, size_t offset=0);



    constexpr static std::size_t elementSize() {
        return 0;
    }
    template <typename T, typename... Ts>
    constexpr static std::size_t elementSize(const T&, const Ts&... args) {
        return sizeof(T) + elementSize(args...);
    }
    
    template <ConTriviablyCopyable T>
    void addTrivial(T t) {
        static_assert(std::is_fundamental_v<T>, "Only fundamental types are supported.");
        if (!isInstanceLittleEndian()) {
            t = reverseByteOrder(t);
            
            //t = std::byteswap(t);
        }
        addRaw(&t, sizeof(t));
    }
    /*
    if autoalign is set then we align all bytes to the Consts::Binary::BSERIALIZE_DATA_ALIGN
      bool autoalign aligns elements to the correct size but this should not be used because
      when receiving the data you now have to explicitly move the offset at the element,
      this is confusing to anyone reading the function, instead if you have information that
      doesnt match the data add _reserved or _padding field
    */
    std::optional<std::vector<u8>> getFormattedData(bool autoAlign=false);

    static void writeDataToTStream(LibTrident::Tstream::TStreamInfo& tStream,
                const std::vector<u8>& data, i64 seekPos=0, std::ios_base::seekdir seekDir= std::ios::beg);

    static std::vector<u8> readDataFromTStream(LibTrident::Tstream::TStreamInfo& tStream, i64 seekPos,
                u64 size, bool checkAlignment=true);
};

};


