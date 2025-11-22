#pragma once
#include <cstdint>
#include <string>
#include "datatypes.h"
namespace LibTrident::Err {
    enum class Code {
            UNDEFINED = 0,
            SUCCESS = 1,
            OK = 1,
            FAIL,
            MALLOC,
            BADARG,
            ACCESS,
            NULL_OBJ,
            FOPEN,
            NOTDIR,
            NOTFILE,
            INVFILE,
            CHMOD,
            COPYOBJ,
            FSEEK,
            FSECNP,
            FSECCRP,
            IO_READ,
            IO_WRITE,
            CHKSUM,
            INVSUID,
            NOSUID,
            ALROPEN,
            IREF_EXPIRED,
            RESV_VIOLATION,
            FNNOTIMPL,
            BADDATA,
            ALIGNMENT
            
    };
    


class TridentError {
protected:
    LibTrident::Err::Code err = LibTrident::Err::Code::UNDEFINED;
public:
    LibTrident::Err::Code GetError() noexcept;
    const std::string_view& GetErrorAsString() noexcept;
    void SetError(LibTrident::Err::Code code) noexcept;
    void SetError(const LibTrident::Err::TridentError& code) noexcept;
    void Success() noexcept;
    bool IsOk() noexcept;

    //throws std::runtime on failure
    void ErrSuccessOrExcept(const std::string& msg);
    //throws std::runtime on failure
    static void ErrSuccessOrExcept(const std::string& msg, Err::Code code);


    static inline const std::string_view& TranslateError(const LibTrident::Err::TridentError& code) noexcept {
        return TranslateError(code.err);    
    }
    //this becomes confusing, rather use wrappers
   // constexpr operator bool() const noexcept {
    //    return err == Err::Code::SUCCESS;
   // }

    static const std::string_view& TranslateError(LibTrident::Err::Code) noexcept;
    friend std::ostream& operator<<(std::ostream& os, const TridentError& m) {
        os << m.TranslateError(m.err);
    return os;

}
};};