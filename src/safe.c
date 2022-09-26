// src/safe.c
// Copyright (C) 2021 Ethan Uppal. All rights reserved.

#include "safe.h"

static void (*fhandler)(enum failure);

void set_failure_handler(void (*handler)(enum failure)) {
    fhandler = handler;
}

void* xmalloc(size_t n) {
    void* ptr = malloc(n);
    if (ptr) {
        return ptr;
    }
    if (!fhandler) {
        fprintf(stderr, "malloc: Virtual memory exhausted\n");
        exit(1);
    } else {
        fhandler(VirtualMemoryExhausted);
        return NULL;
    }
}

FILE* xfopen(const char* filename, const char* mode) {
    FILE* file = fopen(filename, mode);
    if (file) {
        return file;
    }
    if (!fhandler) {
        fprintf(stderr, "fopen: Failed to open file\n");
        exit(1);
    } else {
        fhandler(FailedToOpenFile);
        return NULL;
    }
}
unsigned long xfread(void* buffer, size_t size, size_t count, FILE* stream) {
    unsigned long actual_count = fread(buffer, size, count, stream);
    if (actual_count == count) {
        return count;
    }
    if (!fhandler) {
        fprintf(stderr, "fread: Failed to read file: read %lu objects rather "
                "than the expected %zu\n", actual_count, count);
        exit(1);
    } else {
        fhandler(FailedToReadFile);
        return 0;
    }
}
int xfclose(FILE* file) {
    int status;
    if ((status = fclose(file)) != 0) {
        if (!fhandler) {
            fprintf(stderr, "fclose: Failed to close file");
            exit(1);
        } else {
            fhandler(FailedToCloseFile);
        }
    }
    return status;
}
