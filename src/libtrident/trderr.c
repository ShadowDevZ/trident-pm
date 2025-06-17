#include "trderr.h"
#include <unistd.h>
#include <ctype.h>
#include <string.h>
trd_err_t __trd_err = TRDE_UNSPECIFIED;

const char* __trd_err_msg[] = {
    [TRDE_SUCCESS] = "Operation was completed successfully",
    [TRDE_FAILURE] = "Unknown error has occured",
    [TRDE_BAD_ARG] = "Bad combination of function arguments have been passed to the function",
    [TRDE_UNSPECIFIED] = "The error status has been unspecified",
    [TRDE_MALLOC_FAIL] = "Memory or page allocation has failed",
    [TRDE_IO_FAIL] = "Failed to read/write from the specified resource or descriptor",
    [TRDE_BAD_SEGMENT] = "Invalid package file segment tried to be accessed",
    [TRDE_INV_CHKSUM] = "Failed to verify internal checksum. Possibly a corrupted package",
    [TRDE_BAD_OFFSET] = "The file seek offset is invalid",
    [TRDE_BAD_HDR] = "The package has unrecognized or invalid header format",
    [TRDE_DB_LOCKED] = "The file descriptor belonging to the database is locked. Possibly used by another PM instance",
    [TRDE_IO_ACCESS] = "Could not gain suficient privileges to open the file or package",
    [TRDE_FCB_NONE] = "The address pointing to the callback function is NULL",
    [TRDE_NETW_FAIL] = "Network error when reading or writing packets to the remote location",
    [TRDE_REMOTE_DENY] = "The remote server has denied the operation",
    [TRDE_RES_NOT_FOUND] = "The resource could not be found on the remote server",
    [TRDE_TOKEN_EMPTY] = "The authorization token is empy",
    [TRDE_TOKEN_INV] = "The specified token is invalid",
    [TRDE_REMOTE_TIMEOUT] = "Remote server has not responded in the specified time limit",
    [TRDE_VER_MISSMATCH] = "Remote is unable to fullfil the specified operation.",
    [TRDE_FMT_OUTDATED] = "The package format is no longer supported",
    [TRDE_FMT_UNSUPPORTED] = "Unsupported package format. Possibly an outdated API",
    [TRDE_FILE_CORRUPTED] = "File or segment of it's content is corrupted",
    [TRDE_XMLPARSE] = "Package manifest could not be properly tokenized",
    [TRDE_NOFILE] = "The local file or resource could not be found",
    [TRDE_NULL] = "The pointer was null",
    [TRDE_NOSECTION] = "No section was provided in the source file",
    [TRDE_ALREXISTS] = "Object with requested attributes already exists"

};

trderr_t TRD_GetLastError() {
    return __trd_err;
}

bool TRD_SetLastError(trderr_t err) {
    if (err >= sizeof(__trd_err_msg)/sizeof(__trd_err_msg[0])) {
        return false;

    }
    __trd_err = err;
    return true;
}
const char* TRD_TranslateError(trderr_t err) {
    if (err >= sizeof(__trd_err_msg)/sizeof(__trd_err_msg[0])) {
        return __trd_err_msg[TRDE_FAILURE];
    }
    return __trd_err_msg[err];
}
//arguments
//filepath, rwx="rwx" combination, check access, to check only file
//leave empty
bool CheckFile(char* file, const char* rwx) {
    int modeFlags = F_OK;
    if (strlen(rwx) > 0) {
    
        while ((*rwx)) {
            char c = tolower(*rwx++);
            
            if (c == 'r') {
                modeFlags |= R_OK;
            }
            else if (c == 'w') {
                modeFlags |= W_OK;
            }
            else if (c == 'x') {
                modeFlags |= X_OK;
            }       
        }
    }
    if(!access(file, modeFlags)) {
        return true;
    }

    return false;
}