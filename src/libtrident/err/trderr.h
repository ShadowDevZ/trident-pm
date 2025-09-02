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
            HDRNP,
            HDRCRP,
            IOREAD,
            IOWRITE,
            UNDEFINED = -1,
            
    }RSP;


class TridentError {
protected:
    LTSTATUS::LTSTATUS err = LTSTATUS::SUCCESS;
public:
    LTSTATUS::LTSTATUS GetError();
    std::string GetErrorAsString();
    void SetError(const LTSTATUS::LTSTATUS code);
    void SetError(const LTSTATUS::TridentError& code);
    void Success();
    

    static std::string TranslateError(const LTSTATUS::LTSTATUS code);
    friend std::ostream& operator<<(std::ostream& os, const TridentError& m) {
        os << m.TranslateError(m.err);
    return os;
}

    
};


};