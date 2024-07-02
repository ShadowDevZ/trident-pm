#pragma once
#include <stdint.h>
#include <stddef.h>

#define LIB_TRIDENT_STANDARD 0x1A11313FF



#define __TRIDENT_PACKED __attribute__((__packed__))
#include "trheader.h"

typedef void* TRD_CTX; //<----make it struct with path values, ... 
typedef uint32_t trd_err_t;

typedef void* TRD_PKG;
typedef void* TRD_HDR;
typedef void* TRD_TBL;
trd_err_t TRPX_InitLibrary();
void TRPX_ShutdownLibrary();

TRD_PKG* TRPX_OpenPackage(const char* path, TRD_CTX* ctx);
void TRPX_ClosePackage(TRD_CTX* ctx, TRD_PKG* pkg);
TRD_PKG* TRPX_ReadPackage(TRD_CTX* ctx);


void TRPX_WritePackageHeader(TRD_CTX* ctx, TRD_PKG* pkg, TRD_HDR hdr);
void TRPX_WritePackage(TRD_CTX* ctx, TRD_PKG* pkg);

TRD_TBL* TRPX_GetTable(uint64_t uti, TRD_PKG* pkg);
/*
TRPX_DownloadPackage
TRPX_InstallPackage
TRPX_OpenDatabase
TRPX_WriteDatabasa
TRPX_ReadDatabase
TRPX_SyncDatabase
TRPX_CloseDatabase

TRPX_SET_MIRROR
TRPX_VERIFY_PKG
TRPX_IMPORT_KEYS
TRPX_DECRYPT
TRPX_CheckTime
TRPX_LockDatabase
TRPX_UnlockDatabase

TRPX_InitLib
TRPX_Closelib

openpkg
closepkg
readpkg
writepkg
readheader
writeheader
gettable
writetable
verifyinternalchksum
compresspackage
decompresspackage
encryptpackage
decryptpackage
setchronotime
updatepackages
getspecversion
xmlparseoptionfromheader uses external xml lib, just wrapper
setdownloadagentstring, default is TRIDENT_PM/1.0
utiparse
verifydeveloper
getauthorhandle
*/
bool TRPX_CheckHeader(TRPD_HEADER* hdr);
TRPD_HEADER TRPX_GetHeader();
