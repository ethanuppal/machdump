// main.c
// Copyright (C) 2021 Ethan Uppal. All rights reserved.

#include <stdio.h>
#include <string.h>
#include "include/dump.h"
#include "include/safe.h"

void driver(const char* filename) {
    FILE* file = xfopen(filename, "r");
    xfseek(file, 0, SEEK_END);
    const size_t length = xftell(file);
    xrewind(file);

    void* buffer = xmalloc(length);
    xfread(buffer, sizeof(char), length, file);

    xfclose(file);

    mach_dump(buffer, length);

    xfree(buffer);
}

static void print_help(const char* argv[]) {
    printf("Usage: %s [--help|--version]\n"
           "   or: %s [FILE...]\n"
           "\n"
           "Verbatim dumps 64-bit Mach-O object files for low-level "
           "debugging.\n",
           argv[0], argv[0]);
}

static void print_version(void) {
    printf("Copyright (C) 2021 Ethan Uppal\n"
           "All rights reserved.\n");
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        print_help(argv);
    } else if (argc == 2) {
        if (strncmp(argv[1], "--help", 7) == 0) {
            print_help(argv);
        } else if (strncmp(argv[1], "--version", 10) == 0) {
            print_version();
        } else {
            driver(argv[1]);
        }
    } else {
        for (int i = 1; i < argc; i++) {
            driver(argv[i]);
        }
    }
}
