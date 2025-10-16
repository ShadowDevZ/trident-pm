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
    std::vector<uint8_t> bufferData;
    std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr;
    bool littleEndian = false;
public:
    
    static inline bool IsLittleEndianArch() {
        int i = 1;
        if ((int)*((unsigned char *)&i)==1) {
            return true;
        }
        return false;
    }
    
    inline bool HaveBE() {
        return !littleEndian;
    }
   
    BinarySerializer(std::shared_ptr<LibTrident::Tstream::TStreamInfo> fStreamInfo) :
    wFstr(fStreamInfo), littleEndian(IsLittleEndianArch()) {}

    BinarySerializer(const BinarySerializer& other) : wFstr(other.wFstr), littleEndian(other.littleEndian) {}
    BinarySerializer(BinarySerializer&& other) : wFstr(std::move(other.wFstr)), littleEndian(other.littleEndian) {}
            
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
    static inline bool IsDataSizeAligned(size_t size, bool die=true) {
        if (size % LibTrident::Consts::Binary::BSERIALIZE_DATA_ALIGN != 0) {
            if (die) {
                dassert(0 && "Data must be 8 byte aligned");
            }
            return false;
        }
        return true;
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

    bool AddRaw(const void*  data, size_t size);
    
    template <typename T>
    bool AddTrivial(T t) {
        static_assert(std::is_trivially_copyable_v<T>, "Only trivially copyable types are supported.");
        if (HaveBE()) {
            t = ReverseByteOrder(t);
        }
        return AddRaw(&t, sizeof(t));
    }

    bool WriteData(i64 seekPos, std::ios_base::seekdir seekDir=std::ios::beg);
   

};

};


