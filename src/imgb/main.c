#include "stdio.h"
#include "builder.h"
#include "libtrident.h"
int main() {
    printf("Trident Package Builder %s\n", TRD_BUILDER_VERSION);
    TRD_WriteHeader();
    return 0;
}