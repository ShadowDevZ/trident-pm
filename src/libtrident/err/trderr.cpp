#include "trderr.h"
#include <array>
#include <string>
#include <string_view>
#include <ccattribs.h>
using namespace LibTrident;
using namespace LibTrident::Err;

//we cannot use std::string as it occupies heap
/*Originally I wanted to use unordered_map, but it uses dynamic allocations and cannot be marked with constexpr
We have to use this messy workaround, If you are also asking why am i converting this into an std::aray instead
of straight up declaring it. The answer is that I found out that in C++17 you have to explicitly tell the size of the array even
if its marked as constexpr. This is fixed inside C++20+ but we are currently stuck to C++17. You can do it without the size but NOT if you
are embedding struct or using std::pair, also we cannot do this because it makes only 1 element array yay
//constexpr std::array gErrorMessages {_errmsgTbl};
*/
constexpr std::pair<Err::Code,std::string_view> gErrorMessages [] = {
    {Err::Code::UNDEFINED, "Undefined error"},
    {Err::Code::OK, "Operation was successful"},
    {Err::Code::FAIL, "The call to the specified function has failed"},
    {Err::Code::MALLOC, "Memory allocation has failed"},
    {Err::Code::BADARG, "Unknown or incorrect argument has been passed to the function"},
    {Err::Code::ACCESS, "Insufficient privileges to the specified resource, access denied"},
    {Err::Code::NULL_OBJ, "The object was NULL"},
    {Err::Code::FOPEN, "Error opening access handle"},
    {Err::Code::NOTDIR, "Filesystem object is not a directory"},
    {Err::Code::INVFILE, "Filesystem object does not exist"},
    {Err::Code::CHMOD, "Failed to change permissions"},
    {Err::Code::COPYOBJ, "Failed to copy object"},
    {Err::Code::FSEEK, "Failed to set seek pointer"},
    {Err::Code::FSECNP, "File section is missing"},
    {Err::Code::FSECCRP, "File section tags are corrupted"},
    {Err::Code::IO_READ, "Failed to ReadHeader file"},
    {Err::Code::IO_WRITE, "Failed to WriteHeader file"},
    {Err::Code::CHKSUM, "CRC32 checksum has failed"},
    {Err::Code::NOSUID, "SUID token not found"},
    {Err::Code::INVSUID, "Invalid SUID token"},
    {Err::Code::IREF_EXPIRED, "Internal reference to object has expired. Context is lost"},
    {Err::Code::RESV_VIOLATION, "Reserved field not set to 0"},
    {Err::Code::ALROPEN, "Object was already opened"},
    {Err::Code::FNNOTIMPL, "Function not implemented. Do not use"},
    {Err::Code::BADDATA, "Bad data was passed to the function"},
    {Err::Code::ALIGNMENT, "Data was not properly aligned before written. Alignment violated"}
};



Err::Code Err::TridentError::GetError() {
    return err;
}
bool Err::TridentError::IsOk() {
    if (err == Err::Code::SUCCESS) {
        return true;
    }
    return false;
}

void Err::TridentError::SetError(const Err::Code code) {
    err = code;
}
void Err::TridentError::SetError(const Err::TridentError& code) {
    err = code.err;
}
void Err::TridentError::Success() {
    err = Err::Code::SUCCESS;
}
const std::string_view& Err::TridentError::TranslateError(const Err::Code code) {
   
   for (const auto&  x: gErrorMessages)  {
        if (x.first == code) {
            
            return x.second;
        }
   }
   
   return gErrorMessages[0].second;
}
const std::string_view& Err::TridentError::GetErrorAsString() {
    return TridentError::TranslateError(TridentError::GetError());
}