#include "io.h"

char* ReadFileToBuffer(char* filename, size_t* sizeOut) {
    char* buff = NULL;
    if (!filename) {
        return NULL;
    }
    FILE* f = fopen(filename, "r");
    if (!f) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    if (size <= 0) {
        fclose(f);
        return NULL;
    }
    buff = malloc(size+1);
    if (!buff) {
        fclose(f);
        return NULL;
    }
    size_t readSize = fread(buff, 1, size, f);

    if (sizeOut != NULL) {
        *sizeOut = readSize;
    }
    return buff;

}