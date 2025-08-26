#include "trderr.h"
#include <unordered_map>
#include <string>
const std::unordered_map<LTSTATUS::LTSTATUS, std::string> ErrorMessages = {
    {LTSTATUS::OK, "Success"},
    {LTSTATUS::FAIL, "The call to the specified function has failed"},
    {LTSTATUS::MALLOC, "Memory allocation has failed"},
    {LTSTATUS::BADARG, "Unknown or incorrect argument has been passed to the function"},
    {LTSTATUS::ACCESS, "Insufficient privileges to the specified resource, access denied"},
    {LTSTATUS::NULL_OBJ, "The object was NULL"},
    {LTSTATUS::FOPEN, "Error opening access handle"},
    {LTSTATUS::UNDEFINED, "Undefined error"},
    {LTSTATUS::NOTDIR, "Filesystem object is not a directory"},
    {LTSTATUS::INVFILE, "Filesystem object does not exist"},
    {LTSTATUS::CHMOD, "Failed to change permissions"}
};

LTSTATUS::LTSTATUS LTSTATUS::TridentError::GetError() {
    return err;
}
void LTSTATUS::TridentError::SetError(LTSTATUS::LTSTATUS code) {
    err = code;
}
std::string LTSTATUS::TridentError::TranslateError(LTSTATUS::LTSTATUS code) {
    auto msg = ErrorMessages.find(code);
    if (msg != ErrorMessages.end()) {
        return msg->second;
    }
    return "Undefined error";
}
std::string LTSTATUS::TridentError::GetErrorAsString() {
    return TridentError::TranslateError(TridentError::GetError());
}