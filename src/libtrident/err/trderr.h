#pragma once
#include <cstdint>
#include <string>
namespace LTSTATUS {


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
            UNDEFINED = -1
    }RSP;


class TridentError {
protected:
    LTSTATUS::LTSTATUS err = LTSTATUS::UNDEFINED;
public:
    LTSTATUS::LTSTATUS GetError();
    std::string GetErrorAsString();
    void SetError(LTSTATUS::LTSTATUS code);
    static std::string TranslateError(LTSTATUS::LTSTATUS code);
};

}