/**
 * @file trderr.h
 * @brief Provides class to store library error information 
 * 
 * 
 */
#pragma once
#include <cstdint>
#include <string>
#include "datatypes.h"
#include "trdconsts.h"
#include <memory>
#include <optional>
namespace LibTrident::Err {
    /**
     * @brief Errorcodes when additional information is needed
     * 
     */
    enum class Code {
            /// This error should not be returned, appropriate error type should be used
            UndefinedError = 0,
            /// Operation was completed successfully
            Success = 1,
            /// Indicates a non standard error code which holds information in SecondaryErrorInfo class
            CustomError,
            /// Improper argument types or values were passed to the called function
            InvalidFuncArg,
            /// Tried opening file with insufficient privileges
            FileAccessFailure,
            /// Internal object which was supposed to be initialized doesn't point to any valid data
            NullObject,
            /// Indicates error where file handle could not be properly opened
            FileOpenFailure,
            /// The passed file system object is not a directory
            ObjectNotDir,
            /// The passed file system object is not a file
            ObjectNotFile,
            /// Provided file was not found
            FileMissing,
            /// Failed to change or update file attributes
            FileAttrModFailure,
            /// Could not copy filesystem object to specified destination
            ObjectCopyFailure,
            /// Seek file operation failed
            StreamSeekFailure,
            /// Section is not present but it should have
            SectionMissing,
            /// Block of non optional data where file section was supposed to be is fully or partially corrupted 
            SectionCorrupted,
            /// Read operation failed
            FileReadFailure,
            /// Write operation failed
            FileWriteFailure,
            /// CRC32 checksum validation of buffer does not match with the expected value
            ChecksumFailure,
            /// UUID for section is not equal to the expexted value
            SUIDInvalid,
            /// UUID for section is not present at all
            SUIDMissing,
            /// Tried opening already opened and locked file
            FileAlrOpen,
            /// Internal reserved struct field contained non zero value
            ReservedFieldViolated,
            /// The function is not implemented. This error should be used only for testing builds
            FunctionNotImplemented,
            /// Data type is not properly aligned according to BSERIALIZE_DATA_ALIGN constant
            UnalignedData,
            /// Code execution failed as the host operating system is not supported or does not support certain system function
            UnsupportedPlatform,
            /// The object which was passed/returned to/from function does not hold correct data value
            BadObject,
            /// scheduled for removal
            ReferenceExpired,
            /// Call to operating system specific function or syscall failed with an error
            OSFunctionCallFailed,
            /// Provided data could not be properly serialized/deserialized
            SerializerFailure
            
    };
    /*
    Extended error info, used when for example we have SectionMissingError
    we could then be using secondaryError field to mark which section is missing,
    for example HeaderSection, the error message provides arbitrary information
    which could include warning information like when caller forgets to call some important function.
    This should generally include user friendly error. Maximum string size is defined by
    SECONDARY_ERROR_MAXSIZE constant defined in <trconsts.h>. This is only there to prevent
    whole stack traces being stored there and to prevent making unnecessarily large objects
    */
    

    /**
     * @brief Stores extended error information
     * @details  Extended error info, used when for example we have SectionMissingError
     * we could then be using secondaryError field to mark which section is missing,
     * for example HeaderSection, the error message provides arbitrary information
     * which could include warning information like when caller forgets to call some important function.
     * This should generally include user friendly error. Maximum string size is defined by
     * SECONDARY_ERROR_MAXSIZE constant defined in <trconsts.h>. This is only there to prevent
     * whole stack traces being stored there and to prevent making unnecessarily large objects
     */
    struct SecondaryErrorInfo {
        /**
         * @brief Custom user defined error 
         * @details In case you are trying to provide additional clear error message as to what
         * caused the primaryerror to occur just set the secondaryError to the error code from Code class
         */
        u32 secondaryError{0};
        /// user friendly error message with maximum size of SECONDARY_ERROR_MAXSIZE
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
        setError(primary, extErr, extErrMsg);
    };
    LibTrident::Err::Code getError() const noexcept {
        return primaryError;
    }
    /// @brief returns stored secondary error info
    /// @return secondary info if present
    std::optional<SecondaryErrorInfo> getSecondaryError() const noexcept {
        return secondaryError;
    }
    
    void setError(Err::Code primary) {
        primaryError = primary;
    }
    void setError(Err::Code primary, u32 extErr=0, std::string extErrMsg="") {
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
    ///  checks whether the stored error code is Code::Success
    bool isOK() const noexcept {
        return (primaryError == Err::Code::Success);
    }
    /// sets stored error to Code::Success and clears secondaryError
    void setSuccess() {
        primaryError = Err::Code::Success;
        //unset any other flags as secondaryError is not available for all errors
        secondaryError.reset();
    }
    /// returns user friendly primary error as string
    std::string_view getErrorAsString() const noexcept {
        return TrdError::translateError(primaryError);
    }
    /// @brief translates primary error code to string
    /// @param primary primary error code
    /// @return translated string
    static std::string_view translateError(LibTrident::Err::Code primary) noexcept;

    static std::string_view translateError(const TrdError& trdErr) noexcept {
        return translateError(trdErr.primaryError);
    } 
    friend std::ostream& operator<<(std::ostream& os, const TrdError& m)  {
        os << m.translateError(m.primaryError);
        return os;
    }
    operator bool() const {
        return isOK();
    }

};

};
