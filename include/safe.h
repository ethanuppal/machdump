// include/safe.h
// Copyright (C) 2021 Ethan Uppal. All rights reserved.

#pragma once

#include <stdio.h>
#include <stdlib.h>

enum failure {
    VirtualMemoryExhausted,
    FailedToOpenFile,
    FailedToReadFile,
    FailedToCloseFile
};

void set_failure_handler(void (*handler)(enum failure));

void* xmalloc(size_t n);
#define xfree free

FILE* xfopen(const char* filename, const char* mode);
unsigned long xfread(void* buffer, size_t size, size_t count, FILE* stream);
int xfclose(FILE* file);

#define xfseek fseek
#define xftell ftell
#define xrewind rewind
