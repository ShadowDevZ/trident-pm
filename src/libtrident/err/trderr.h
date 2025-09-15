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
            
    };


class TridentError {
protected:
    LibTrident::Err::Code err = LibTrident::Err::Code::UNDEFINED;
public:
    LibTrident::Err::Code GetError();
    const std::string_view& GetErrorAsString();
    void SetError(LibTrident::Err::Code code);
    void SetError(const LibTrident::Err::TridentError& code);
    void Success();
    bool IsOk();

    static inline const std::string_view& TranslateError(const LibTrident::Err::TridentError& code) {
        return TranslateError(code.err);    
    }

    static const std::string_view& TranslateError(LibTrident::Err::Code);
    friend std::ostream& operator<<(std::ostream& os, const TridentError& m) {
        os << m.TranslateError(m.err);
    return os;

}
};};