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
           
            std::streamsize GetFstreamSize(std::shared_ptr<std::fstream> fs);
            
            ///The following 2 functions format the buffer and WriteHeader it as Little endian
            //does not increment fSize
           
            
            bool WriteLeData(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size);
            //increments fSize by bytes written by default, if updating alReadHeadery written variable INCREMENT MUST BE FALSE
            bool ReadLeData(std::shared_ptr<std::fstream> stream, char* s, std::streamsize size);
             
            bool GetFileStats(const std::filesystem::path& file, struct stat64& statOut);

            
    };
   

    class Descriptor {
        public:
            virtual ~Descriptor() = default;
            //beg=true mean beginning of section, beg=false end of section
            virtual bool WriteDescriptorSUID() = 0;
            virtual bool ReadDescriptorSUID() = 0;
            virtual bool  IsValidSUID() = 0;
           
    };
    //serializes struct to array of bytes
    //each functiopn which wants to utilize TStream and use non trivial datatypes must implement Serialize() function
    class BinarySerializer {
private:
    std::vector<uint8_t> bufferData {};
  //  std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr;
    bool littleEndian = false;
public:
    
    static inline bool IsLittleEndianArch() {
#if LT_ENDIAN_FORCE != 0
        #if LT_ENDIAN_FORCE == 1
            return true;
        #else
            return false;
        #endif

#elif LT_ENDIAN_CHECK_RT == 1
        int i = 1;
        if ((int)*((unsigned char *)&i)==1) {
            return true;
        }
        return false;


#else
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return false;
    #else
        return true
    #endif
    
#endif
    }
    
    inline bool HaveBE() {
        return !littleEndian;
    }
    BinarySerializer() : littleEndian(IsLittleEndianArch()) {}
    //second constructor for deserialize()
    BinarySerializer(const std::vector<uint8_t>& data) : bufferData(data), littleEndian(IsLittleEndianArch()) {}

    BinarySerializer(const BinarySerializer& other) : bufferData(other.bufferData), littleEndian(other.littleEndian) {}
    BinarySerializer(BinarySerializer&& other) : bufferData(other.bufferData), littleEndian(other.littleEndian) {}
            
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
        //make the datatype 8 byte aligned
            
    static size_t GetByteAlignment(size_t varSize) {
              
        const auto& alignBytes = Consts::Binary::BSERIALIZE_DATA_ALIGN;
                
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
    static inline bool IsDataSizeAligned(size_t size) {
        return size % LibTrident::Consts::Binary::BSERIALIZE_DATA_ALIGN == 0;
    }

    //If data is ok returns true otherwise false
    //same as IsDataSizeAligned except that this leaves no room for fixing the size by padding it
    //kills program if size is not properly aligned
    static inline bool ExpectAlignedDataOrDie(size_t size) {
        //normal assert used because this condition simply cant happen
        bool aligned = IsDataSizeAligned(size);
        assert(aligned && "Data must be 8 byte aligned");
        
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
    LT_UNSAFE_API bool AddRaw(const void*  data, size_t size);
    
   
    //C styled array override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <typename T, std::size_t N>
    bool AddType(const T (&arr)[N]) {
        if (N < 1) {
            return false;
        }

        for (std::size_t i = 0; i < N; ++i) {
            if (!AddTrivial(arr[i])) {return false;}
        }
        return true;
    }
    //std::array override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <typename T, std::size_t N>
    bool AddType(const std::array<T, N>& arr) {
        if (N < 1) {
            return false;
        }

        for (const auto& v : arr) {
            if (!AddTrivial(v)) {return false;}
        }
        return true;
    }
    //std::vector override, only for fundamental types
    //if your arrays uses non fundamental type please define your own serialize method
    template <typename T>
    bool AddType(const std::vector<T>& vec) {
        if (vec.size() < 1) {
            return false;
        }

        for (const auto& v : vec) {
            if (!AddTrivial(v)) {return false;}
        }
        return true;
    }

    //todo
    //reads sizeof T from bufferData and writes to ptrOut
    template <typename T>
    bool ReadTrivial(T* t) {
        (void)(t);
        return false;
    }
    
    
    template <typename T>
    bool AddTrivial(T t) {
        static_assert(std::is_fundamental_v<T>, "Only fundamental types are supported.");
        if (HaveBE()) {
            t = ReverseByteOrder(t);
        }
        return AddRaw(&t, sizeof(t));
    }
    //if autoalign is set then we align all bytes to the Consts::Binary::BSERIALIZE_DATA_ALIGN
    std::optional<std::vector<u8>> GetFormattedData(bool autoAlign);
    static Err::Code WriteDataToTStream(std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr, const std::vector<u8>& data, i64 seekPos=0, std::ios_base::seekdir seekDir= std::ios::beg);

};

};


