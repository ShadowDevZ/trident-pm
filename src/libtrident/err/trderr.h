#pragma once
#include <cstdint>
#include <string>
#include "datatypes.h"
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
    
class TrdError {
protected:
    LibTrident::Err::Code err {LibTrident::Err::Code::Success};
public:
    TrdError() {};
    TrdError(Err::Code code) : err(code) {};
    LibTrident::Err::Code GetError() const noexcept {
        return err;
    }
    void SetError(Err::Code code) {
        err = code;
    }
    bool IsOK() const noexcept {
        return (err == Err::Code::Success);
    }
    void SetSuccess() {
        err = Err::Code::Success;
    }
    //stringview is nonowning, so no reference
    std::string_view GetErrorAsString() const noexcept {
        return TrdError::TranslateError(err);
    }
    static std::string_view TranslateError(LibTrident::Err::Code code) noexcept;

    static std::string_view TranslateError(const TrdError& trdErr) noexcept {
        return TranslateError(trdErr.err);
    }
    friend std::ostream& operator<<(std::ostream& os, const TrdError& m)  {
        os << m.TranslateError(m.err);
        return os;
    }
    operator bool() const {
        return IsOK();
    }

};

};
