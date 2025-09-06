#include "trderr.h"
#include <array>
#include <string>
#include <ccattribs.h>
using namespace LibTrident;

//we cannot use std::string as it occupies heap
/*Originally I wanted to use unordered_map, but it uses dynamic allocations and cannot be marked with constexpr
We have to use this messy workaround, If you are also asking why am i converting this into an std::aray instead
of straight up declaring it. The answer is that I found out that in C++17 you have to explicitly tell the size of the array even
if its marked as constexpr. This is fixed inside C++20+ but we are currently stuck to C++17. You can do it without the size but NOT if you
are embedding struct or using std::pair, also we cannot do this because it makes only 1 element array yay
//constexpr std::array gErrorMessages {_errmsgTbl};
*/
constexpr std::pair<LTSTATUS::LTSTATUS,const char*> gErrorMessages [] = {
    {LTSTATUS::OK, "Operation was successful"},
    {LTSTATUS::FAIL, "The call to the specified function has failed"},
    {LTSTATUS::MALLOC, "Memory allocation has failed"},
    {LTSTATUS::BADARG, "Unknown or incorrect argument has been passed to the function"},
    {LTSTATUS::ACCESS, "Insufficient privileges to the specified resource, access denied"},
    {LTSTATUS::NULL_OBJ, "The object was NULL"},
    {LTSTATUS::FOPEN, "Error opening access handle"},
    {LTSTATUS::UNDEFINED, "Undefined error"},
    {LTSTATUS::NOTDIR, "Filesystem object is not a directory"},
    {LTSTATUS::INVFILE, "Filesystem object does not exist"},
    {LTSTATUS::CHMOD, "Failed to change permissions"},
    {LTSTATUS::COPYOBJ, "Failed to copy object"},
    {LTSTATUS::FSEEK, "Failed to set seek pointer"},
    {LTSTATUS::HDRNP, "Header is missing"},
    {LTSTATUS::HDRCRP, "Header data tags corrupted"},
    {LTSTATUS::IOREAD, "Failed to read file"},
    {LTSTATUS::IOWRITE, "Failed to write file"},
    {LTSTATUS::CHKSUM, "CRC32 checksum has failed"},
    {LTSTATUS::NOTUID, "TUID token not found"},
    {LTSTATUS::INVTUID, "Invalid TUID token"}
};




LTSTATUS::LTSTATUS LTSTATUS::TridentError::GetError() {
    return err;
}
void LTSTATUS::TridentError::SetError(const LTSTATUS::LTSTATUS code) {
    err = code;
}
void LTSTATUS::TridentError::SetError(const LTSTATUS::TridentError& code) {
    err = code.err;
}
void LTSTATUS::TridentError::Success() {
    err = SUCCESS;
}
std::string LTSTATUS::TridentError::TranslateError(const LTSTATUS::LTSTATUS code) {
   
   for (auto&&  x: gErrorMessages)  {
        if (x.first == code) {
            std::string errMsg(x.second);
            return errMsg;
        }
   }
   
   return "Undefined Error";
}
std::string LTSTATUS::TridentError::GetErrorAsString() {
    return TridentError::TranslateError(TridentError::GetError());
}