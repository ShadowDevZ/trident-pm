#pragma once
#include <cstdint>
#include <string>
namespace LibTrident::LTSTATUS {


    typedef uint32_t LTSTATUS;

    typedef enum {
            SUCCESS = 0,
            OK = 0,
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
            IREF_EXPIRED,
            RESV_VIOLATION,
            UNDEFINED = -1,
            
    }RSP;


class TridentError {
protected:
    LTSTATUS::LTSTATUS err = LTSTATUS::SUCCESS;
public:
    LTSTATUS::LTSTATUS GetError();
    const char* GetErrorAsString();
    void SetError(const LTSTATUS::LTSTATUS code);
    void SetError(const LTSTATUS::TridentError& code);
    void Success();
    bool IsOk();

    static inline const char* TranslateError(const TridentError& code) {
        return TranslateError(code.err);    
    }

    static const char* TranslateError(const LTSTATUS::LTSTATUS code);
    friend std::ostream& operator<<(std::ostream& os, const TridentError& m) {
        os << std::string(m.TranslateError(m.err));
    return os;
}

    
};


};