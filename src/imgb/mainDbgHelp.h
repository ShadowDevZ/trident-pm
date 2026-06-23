#pragma once
#include <expected>
#include <type_traits>
#include "trderr.h"
#include <vector>
#include <iostream>
#include "sdescdefs.h"
#include "trheader.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include <chrono>
//only included in main file during testing so this is ok
using namespace Trd;

#define TASSERT(name, expr) tassert(name, [&] { return expr; })

template <typename T, typename E>
struct is_expected_with_error : std::false_type {};

template <typename V, typename E>
struct is_expected_with_error<std::expected<V, E>, E> : std::true_type {};

template <typename NAME, typename FUNC>
    requires std::convertible_to<NAME, std::string_view>

/**
 * @brief Debug function for asserting the Trd function in the main file
 * @attention Consider using the TASSERT macro instead which automatically converts
 * the function to the lambda expression
 * @details In debug mode this function checks not only the return value but also the 
 * std::expected error. In release mode this function simply executes
 * whatever was passsed as the 'function' parameter
 * @param fnName Text to be displayed eg. "MyFunc()".
 * @param function Function to be evaluated. Passed as lambda expression.
 */
void tassert(NAME fnName, FUNC function) {

#ifdef _LIBTRIDENT_DEBUG
    using ReturnType = std::invoke_result_t<FUNC>;
    //asserting non void functiops doesnt make any sense
    static_assert(!std::is_void_v<ReturnType>,
                  "tassert: Cannot assert void function. Function must "
                  "return an assignable value");

    auto expr = function();
    const std::string_view sv(fnName);

    if (!(expr)) {
        std::cerr << "\x1B[31m" << fnName << std::setw(22 - sv.size()) << "[fail]" << "\n";
        if constexpr (is_expected_with_error<ReturnType, Err::TrdError>::value) {
            std::cerr << "  ::REASON? " << expr.error() << "\n";
        }
        //color reset
        std::cerr << "\x1B[0m";

        std::exit(1);
    } else {
        std::cout << "\x1B[32m" << fnName << std::setw(22 - sv.size()) << "[ok]"
                  << "\x1B[0m\n";
    }
#else
    (void)fnName;
    function();
#endif
}

inline void PrintBuildTarget() {
#ifdef _LIBTRIDENT_DEBUG
    std::cout << "Target: Debug\n\n";
#else
    std::cout << "Target: Release\n";
#endif
}

#ifdef _LIBTRIDENT_DEBUG

inline void print_header(const Trd::TRD_HEADER& hdr) {
    dbgprintf("[HEADER_START - SIZE(mem:%luB, disk:%luB)]\n", sizeof(hdr), hdr.size());
    dbgprintf("\tMagic: [ ");
    for (auto const& it : hdr.magic) {
        dbgprintf("%X ", static_cast<u8>(it));
    }
    dbgprintf("]\n");
    dbgprintf("\tExtened Signature: 0x%X\n", hdr.exSignature);
    auto hdrFmtVal = TrFileHeader::headerVersionFormatToString(hdr.fmtVersion);
    if (!hdrFmtVal.has_value()) {
        abort();
    }

    dbgprintf("\tVersion Format %s\n", hdrFmtVal.value().c_str());
    dbgprintf("\tCompression: %u\n", hdr.compression);
    dbgprintf("\tBuild flags %u\n", hdr.buildFlags);
    dbgprintf("\tArchitecture %u\n", hdr.architecture);
    dbgprintf("\tChecksum 0x%X\n", hdr.dynHdrChksum);
    dbgprintf("\tFile length 0x%lXB\n", hdr.dynFileLen);
    dbgprintf("[HEADER_END]\n");
}
template <typename T>
void dump_arr(std::span<T> t) {
    constexpr size_t dumpArrElimit = 128;
    size_t szRead = t.size() > dumpArrElimit ? dumpArrElimit : t.size();

    for (size_t i = 0; i < szRead; ++i) {
        dbgprintf("%X%c", static_cast<u8>(t[i]), ((i + 1) % 16 == 0 ? '\n' : ' '));
    }
    if (t.size() > dumpArrElimit)
        dbgprintf("\n...(%ld more)", t.size() - dumpArrElimit);
}

inline void print_sd(const Trd::Impl::TRD_SECTION_DESCRIPTOR& sd) {
    dbgprintf("[SD_START - SIZE(mem:%luB, disk:%luB)]\n", sizeof(sd), sd.size());
    dbgprintf("\tCRC: 0x%X\n", sd.crc);
    dbgprintf("\t  (DTBL)\n");
    dbgprintf("\t    offset: 0x%lX\n", sd.tblDynamic.offset);
    dbgprintf("\t    size: 0x%lX\n", sd.tblDynamic.size);
    dbgprintf("\t    available: %s\n", u8bool::toBool(sd.tblDynamic.available) ? "true" : "false");

    dbgprintf("\t  (TREG)\n");
    dbgprintf("\t    offset: 0x%lX\n", sd.tblRegistry.offset);
    dbgprintf("\t    size: 0x%lX\n", sd.tblRegistry.size);
    dbgprintf("\t    available: %s\n", u8bool::toBool(sd.tblRegistry.available) ? "true" : "false");
    dbgprintf("\tReserved1: %u\n", sd._reserved1);
    dbgprintf("\tReserved2: %lu\n", sd._reserved2.size() * sizeof(std::byte));
    dbgprintf("\tReady status: %s\n", u8bool::toBool(sd.sdReady) ? "true" : "false");
    dbgprintf("\tID byte: 0x%X\n", static_cast<u8>(sd.idByte));
    dbgprintf("[SD_END]\n\n");
}

inline void print_stat(const Trd::Impl::PortableStat& ps) {
    const Trd::u32 perms = static_cast<Trd::u32>(ps.permissions) & 0777;
    if (!ps.auxiliary || !ps.auxiliary.value().times.fileCreated) {
        dbgprintf("print_stat optional missing\n");
        return;
    }
    auto aux = ps.auxiliary.value();

    auto to_time_t = [](auto tp) -> std::time_t {
        return std::chrono::system_clock::to_time_t(tp);
    };
    //ugly debug print, i still dont know how to use std::print, i always get
    //kilometres of unreadable template errors
    //size is expected to be 0 here as we are creating fresh file and data is not written becasue of RAII
    std::cerr << "stat() info\n"
              << "Type: " << static_cast<signed char>(ps.fileType)
              << "\n  Size: " << ps.fileSize.value_or(0) << "\n  Perms: " << std::oct << perms
              << std::dec << "\n  UID: " << aux.optOwnerUID.value_or(0)
              << "\n  GID: " << aux.optOwnerGID.value_or(0)
              << "\n  BTIME: " << to_time_t(aux.times.fileCreated.value())
              << "\n  ATIME: " << to_time_t(aux.times.lastAccess)
              << "\n  CTIME: " << to_time_t(aux.times.lastMetadataChange)
              << "\n  MTIME: " << to_time_t(aux.times.lastModify) << '\n';
}

class BenchDbgTimer {
  private:
    std::string text;
    std::chrono::high_resolution_clock::time_point start;

  public:
    explicit BenchDbgTimer(std::string text) :
        text(std::move(text)), start(std::chrono::high_resolution_clock::now()) {};

    ~BenchDbgTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        dbgprintf("[\x1B[35m%s] took %ld ms/ %ld us\n\x1B[0m", text.c_str(), ms, us);
    }
};

#endif

//test
struct NTC_INFO_TEST : Trd::Impl::SerializableData {
    uint16_t x = 0;
    uint32_t y = 0;
    uint16_t z = 0;

    Trd::u64 size() const override {
        return Trd::Impl::BinarySerializer::elementSize(x, y, z);
    }
    //std::array<uint32_t,2> c{};

    //the sum of sizeof of all elements must be properly aligned
    std::optional<std::vector<std::byte>> serialize() const override {
        Trd::Impl::BinarySerializer bs;
        bs.addTrivial(x); //2B
        bs.addTrivial(y); //4B
        bs.addTrivial(z); //2B
        //  bs.AddType(c);

        return bs.getFormattedData();
    }

    //wip idea   void bs::ReadTrivial<T>(const std::vector<std::byte>& in, const char* outData);
    //if return is false caller throws std::invalid_argument exception
    bool deserialize(const std::vector<std::byte>& dataIn) override {
        //on error throws exception
        Trd::Impl::BinarySerializer bs(dataIn);
        Trd::u64 xsize = 0;
        xsize += bs.readTrivialEx<Trd::u16>(xsize, x);
        xsize += bs.readTrivialEx<Trd::u32>(xsize, y);
        xsize += bs.readTrivialEx<Trd::u16>(xsize, z);

        if (xsize != this->size()) {
            return false;
        }

        // xsize += bs.ReadRaw(&x, sizeof(x), xsize);
        //  xsize += bs.ReadRaw(&y, sizeof(y), xsize);
        //xsize += bs.ReadRaw(&z, sizeof(z), xsize);
        return true;
    }
};
