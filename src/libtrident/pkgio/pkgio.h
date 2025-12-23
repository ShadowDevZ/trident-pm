#pragma once
#include "libtrident.h"
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
namespace LibTrident::PkgIO {
   namespace FileOperations {
            //throws std::ios_base::failure on failure 
            std::streamsize GetFstreamSize(std::weak_ptr<std::fstream> fsx);
            
            ///The following 2 functions format the buffer and WriteHeader it as Little endian
            //does not increment fSize
           
            //throws std::ios::base on failure
            //todo rewrite this shared ptr mess, bad code
            void WriteLeData(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size);
            
            //increments fSize by bytes written by default, if updating alReadHeadery written variable INCREMENT MUST BE FALSE
            //throws std::ios::base, std::bad_alloc, std::runtime_error on failure
            void ReadLeData(std::shared_ptr<std::fstream> stream, char* s, std::streamsize size);
           
            //throws std::system_error on failure
            struct stat64 GetFileStats(const std::filesystem::path& file);

            
    };
   

    class Descriptor {
        public:
            virtual ~Descriptor() = default;
            //beg=true mean beginning of section, beg=false end of section
            virtual std::expected<void, Err::TrdError> WriteDescriptorSUID() = 0;
            virtual std::expected<void, Err::TrdError> ReadDescriptorSUID() = 0;
            virtual bool  IsValidSUID() = 0;
           
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
        static bool IsLittleEndian() {
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
        bool IsInstanceLittleEndian() {
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
    constexpr T ReverseByteOrder(T var) {
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
            
    static size_t GetByteAlignment(size_t varSize) {
              
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
    static constexpr bool IsDataSizeAligned(size_t size) {
        return size % LibTrident::Consts::Binary::BSERIALIZE_DATA_ALIGN == 0;
    }

    //If data is ok returns true otherwise false
    //same as IsDataSizeAligned except that this leaves no room for fixing the size by padding it
    //kills program if size is not properly aligned
    static inline bool ExpectAlignedDataOrDie(size_t size) {
        //normal assert used because this condition simply cant happen
        bool aligned = IsDataSizeAligned(size);
        if (!aligned) {
            throw std::runtime_error("Passed data was not properly aligned");
        }
        //just in case the assertion fails
        return aligned;

    } 
    
    const std::vector<uint8_t>& GetData() const {
        return bufferData;
    }
    std::vector<uint8_t>& GetData() {
        return bufferData;
    }
    inline void EmptyData() {
        bufferData.clear();
    }
    //Warning this method DOES NOT check nor modify endianness
    //Do not use this method unless no other override is available
    //When calling this function you are responsible for passing data in correct endianness
    //throws std::invalid_argument on failure
    LT_UNSAFE_API void AddRaw(const void*  data, size_t size);
    
   
    //C styled array override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <ConTriviablyCopyable T, std::size_t N>
    void AddType(const T (&arr)[N]) {
        if (N < 1) {
            throw std::invalid_argument("Array is empty");
        }

        for (std::size_t i = 0; i < N; ++i) {
            AddTrivial(arr[i]);
        }
    }
    //std::array override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <ConTriviablyCopyable T, std::size_t N>
    void AddType(const std::array<T, N>& arr) {
        if (N < 1) {
            throw std::invalid_argument("Array is empty");
        }

        for (const auto& v : arr) {
            AddTrivial(v);
        }
       
    }
    //std::vector override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <ConTriviablyCopyable T>
    void AddType(const std::vector<T>& vec) {
        if (vec.empty()) {
            throw std::invalid_argument("Vector is empty");
        }

        for (const auto& v : vec) {
            AddTrivial(v);
        }
    }

    //todo
    //reads sizeof T from bufferData and writes to ptrOut
    
    template <typename T>
    //returns number of bytes read
    size_t ReadTrivial(T* t, size_t offset=0) {
        static_assert(std::is_fundamental_v<std::remove_pointer_t<T>>, "Only fundamental types are supported.");
        //nulllptr handlerd here
        size_t readSize = ReadRaw(t, sizeof(*t), offset);
        if (!IsInstanceLittleEndian()) {
            *t = ReverseByteOrder(*t);
           // *t = std::byteswap(*t);
        }
        return readSize;
    }
    
    //Warning this method DOES NOT check nor modify endianness
    //Do not use this method unless no other override is available
    //When calling this function you are responsible for passing data in correct endianness
    //throws std::invalid_argument on failure
    LT_UNSAFE_API size_t ReadRaw(void*  dataOut, size_t size, size_t offset=0);



    constexpr static std::size_t ElementSize() {
        return 0;
    }
    template <typename T, typename... Ts>
    constexpr static std::size_t ElementSize(const T&, const Ts&... args) {
        return sizeof(T) + ElementSize(args...);
    }
    
    template <ConTriviablyCopyable T>
    void AddTrivial(T t) {
        static_assert(std::is_fundamental_v<T>, "Only fundamental types are supported.");
        if (!IsInstanceLittleEndian()) {
            t = ReverseByteOrder(t);
            
            //t = std::byteswap(t);
        }
        AddRaw(&t, sizeof(t));
    }
    /*
    if autoalign is set then we align all bytes to the Consts::Binary::BSERIALIZE_DATA_ALIGN
      bool autoalign aligns elements to the correct size but this should not be used because
      when receiving the data you now have to explicitly move the offset at the element,
      this is confusing to anyone reading the function, instead if you have information that
      doesnt match the data add _reserved or _padding field
    */
    std::optional<std::vector<u8>> GetFormattedData(bool autoAlign=false);

    static void WriteDataToTStream(LibTrident::Tstream::TStreamInfo& tStream,
                const std::vector<u8>& data, i64 seekPos=0, std::ios_base::seekdir seekDir= std::ios::beg);

    static std::vector<u8> ReadDataFromTStream(LibTrident::Tstream::TStreamInfo& tStream, i64 seekPos,
                u64 size, bool checkAlignment=true);
};

};


