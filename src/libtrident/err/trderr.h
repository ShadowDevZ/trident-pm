#pragma once
#include <cstdint>
#include <string>
#include "datatypes.h"
#include "trdconsts.h"
namespace LibTrident::Err {
    
    enum class Code {
            UndefinedError = 0,
            Success = 1,
            GenericFailure,
            InvalidFuncArg,
            FileAccessFailure,
            NullObject,
            FileOpenFailure,
            ObjectNotDir,
            ObjectNotFile,
            FileMissing,
            FileAttrModFailure,
            ObjectCopyFailure,
            StreamSeekFailure,
            SectionMissing,
            SectionCorrupted,
            FileReadFailure,
            FileWriteFailure,
            ChecksumFailure,
            SUIDInvalid,
            SUIDMissing,
            FileAlrOpen,
            ReservedFieldViolated,
            FunctionNotImplemented,
            UnalignedData,
            UnsupportedPlatform,
            BadObject,
            ReferenceExpired,
            OSFunctionCallFailed
            
    };
    /*
    Extended error info, used when for example we have SectionMissingError
    we could then using secondaryError field mark which section is missing,
    for example HeaderSection, the error message provides arbitrary information
    which could include warning like for example the library user forgot certain call.
    This should generally include user friendly error. Maximum size is defined by
    SECONDARY_ERROR_MAXSIZE constant defined in <trconsts.h>. This is only there to prevent
    whole stack traces being stored there and to prevent making unnecessarily large objects
    */
    struct SecondaryErrorInfo {
        u32 secondaryError{0};
        std::string friendlyErrorMessage {""}; 
    };
    
class TrdError {
protected:
    LibTrident::Err::Code primaryError {LibTrident::Err::Code::Success};
    std::optional<SecondaryErrorInfo> secondaryError;

public:
    TrdError() {};
    explicit TrdError(Err::Code primary) : primaryError(primary) {};

    explicit TrdError(Err::Code primary, u32 extErr, std::string extErrMsg)  {
        SetError(primary, extErr, extErrMsg);
    };
   
    LibTrident::Err::Code GetError() const noexcept {
        return primaryError;
    }
    std::optional<SecondaryErrorInfo> GetSecondaryError() const noexcept {
        return secondaryError;
    }

    void SetError(Err::Code primary) {
        primaryError = primary;
    }
    void SetError(Err::Code primary, u32 extErr=0, std::string extErrMsg="") {
        if (extErrMsg.size() > LibTrident::Consts::Err::SECONDARY_ERROR_MAXSIZE) {
            throw std::length_error("Error message exceeded max allowed size");
        }
        primaryError = primary;
        secondaryError = SecondaryErrorInfo{
            .secondaryError = extErr,
            //moving because we might or might not modify the message in some way in the future
            .friendlyErrorMessage = std::move(extErrMsg)
        };
        
    }
    bool IsOK() const noexcept {
        return (primaryError == Err::Code::Success);
    }
    void SetSuccess() {
        primaryError = Err::Code::Success;
        //unset any other flags as secondaryError is not available for all errors
        secondaryError.reset();
    }
    //stringview is nonowning, so no reference
    std::string_view GetErrorAsString() const noexcept {
        return TrdError::TranslateError(primaryError);
    }
    static std::string_view TranslateError(LibTrident::Err::Code primary) noexcept;

    static std::string_view TranslateError(const TrdError& trdErr) noexcept {
        return TranslateError(trdErr.primaryError);
    }
    friend std::ostream& operator<<(std::ostream& os, const TrdError& m)  {
        os << m.TranslateError(m.primaryError);
        return os;
    }
    operator bool() const {
        return IsOK();
    }

};

};
