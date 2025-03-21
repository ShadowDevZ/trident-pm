#include "libtrident.h"
#include "trheader.h"
trderr_t _TRD_DtblOffsetTableInit();
trderr_t _TRD_DtblOffsetTableFree();
trderr_t TRD_OpenPackage(const char* path, _TRD_PKGI* pkg) {
    if (pkg == NULL) {
        return TRDE_NULL;
    }
    if (pkg->pkgHandle != NULL) {
        return TRDE_ALREXISTS;
    }
    FILE* f = fopen(path, "wb+");
    if (f == NULL) {
        return TRDE_IO_ACCESS;
    }
    pkg->pkgHandle = f;
    _TRD_DtblOffsetTableInit();
    return TRDE_SUCCESS;
}
trderr_t TRD_ClosePackage(_TRD_PKGI* pkg) {
    if (pkg == NULL) {
        return TRDE_NULL;
    }
    if (pkg->pkgHandle == NULL) {
        return TRDE_NOFILE;
    }
    pkg->hdr.lock = false;
    if (!fclose(pkg->pkgHandle)) {
        return TRDE_SUCCESS;
    }
    _TRD_DtblOffsetTableFree();
    return TRDE_IO_FAIL;
}

