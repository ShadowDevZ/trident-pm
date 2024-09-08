#pragma once
#include <stdint.h>
#include <stddef.h>
#define _INTERNALF_
#define LIB_TRIDENT_STANDARD 0x1A11313FF

typedef enum {
    TRD_CT_NONE = 2,
    TRD_CT_GZIP = 4,
    TRD_CT_LZ4 = 6,
    TRD_CT_XZ = 10
}TRD_COMPRESS_TYPE;


#define __STRUCT_PACK __attribute__((__packed__))
#include "trheader.h"
#include "trderr.h"

typedef void* TRD_CTX; //<----make it struct with path values, ... 
typedef uint32_t trd_err_t;

typedef void* TRD_PKG;
typedef void* TRD_HDR;
typedef void* TRD_TBL;
trd_err_t TRD_InitLibrary();
void TRD_ShutdownLibrary();

TRD_PKG* TRD_OpenPackage(const char* path, TRD_CTX* ctx);
void TRD_ClosePackage(TRD_CTX* ctx, TRD_PKG* pkg);
TRD_PKG* TRD_ReadPackage(TRD_CTX* ctx);


void TRD_WritePackageHeader(TRD_CTX* ctx, TRD_PKG* pkg, TRD_HDR hdr);
void TRD_WritePackage(TRD_CTX* ctx, TRD_PKG* pkg);

TRD_TBL* TRD_GetTable(uint64_t uti, TRD_PKG* pkg);
/*
TRD_DownloadPackage()
TRD_InstallPackage()
TRD_OpenDatabase()
TRD_WriteDatabasa()
TRD_ReadDatabase()
TRD_SyncDatabase()
TRD_CloseDatabase()

TRD_SET_MIRROR
TRD_VERIFY_PKG
TRD_IMPORT_KEYS
TRD_DECRYPT
TRD_CheckTime
TRD_LockDatabase
TRD_UnlockDatabase


TRD_OpenPkg
TRD_ClosePkg
TRD_ReadPkg;
TRD_WritePkg;

TRD_ReadHeader;
TRD_WriteHeader
TRD_ReadTable;
TRD_WriteTable

TRD_CompressPkg
TRD_DecompressPkg
TRD_EncryptPkg
TRD_DecryptPkg
TRD_AttrSetChrono
TRD_ParseDBEntry
TRD_GetFormatVersion
xmlparseoptionfromheader uses external xml lib, just wrapper
setdownloadagentstring, default is TRIDENT_PM/1.0
utiparse
verifydeveloper
getauthorhandle
*/
#include "trheader.h"


