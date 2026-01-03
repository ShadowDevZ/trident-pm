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
using eCode = Err::Code;
constexpr std::pair<Err::Code,std::string_view> gErrorMessages [] = {
    {eCode::UndefinedError, "Undefined error"},
    {eCode::Success, "Operation was successful"},
    {eCode::GenericFailure, "The call to the specified function has failed"},
    {eCode::InvalidFuncArg, "Unknown or incorrect argument has been passed to the function"},
    {eCode::FileAccessFailure, "Insufficient privileges to the specified resource, access denied"},
    {eCode::NullObject, "The object was NULL"},
    {eCode::FileOpenFailure, "Error opening access handle"},
    {eCode::ObjectNotDir, "Filesystem object is not a directory"},
    {eCode::ObjectNotFile, "Filesystem object does not exist"},
    {eCode::FileAttrModFailure, "Failed to change permissions"},
    {eCode::ObjectCopyFailure, "Failed to copy object"},
    {eCode::StreamSeekFailure, "Failed to set seek pointer"},
    {eCode::SectionMissing, "File section is missing"},
    {eCode::SectionCorrupted, "Section contains invalid data"},
    {eCode::FileReadFailure, "Failed to read file"},
    {eCode::FileWriteFailure, "Failed to write file"},
    {eCode::ChecksumFailure, "CRC32 checksum has failed"},
    {eCode::SUIDMissing, "SUID token not found"},
    {eCode::SUIDInvalid, "Invalid SUID token"},
    {eCode::ReservedFieldViolated, "Reserved field not set to 0"},
    {eCode::FileAlrOpen, "Object was already opened"},
    {eCode::FunctionNotImplemented, "Function not implemented. Do not use"},
    {eCode::UnalignedData, "Data was not properly aligned before written. Alignment violated"},
    {eCode::BadObject, "Object does not hold correct size/data"},
    {eCode::ReferenceExpired, "Reference object has expired"},
    {eCode::OSFunctionCallFailed, "Call to operating system function/syscall or routine failed"}
};

std::string_view Err::TrdError::TranslateError(LibTrident::Err::Code code) noexcept {
   
   for (const auto&  x: gErrorMessages)  {
        if (x.first == code) {
            
            return x.second;
        }
   }
   
   return gErrorMessages[0].second;
}
