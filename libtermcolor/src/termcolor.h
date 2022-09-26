// libtermcolor: termcolor.h
// Copyright (C) 2021 Ethan Uppal. All rights reserved.

#ifndef _LIBTERMCOLOR_TERMCOLOR_H
#define _LIBTERMCOLOR_TERMCOLOR_H

#include <stdbool.h>
#include <stdio.h>

// A type describing an libtermcolor error. These are the only value returned by
// the tcol_fprintf and tcol_printf functions. You may assume any nonzero value
// is an error and use tcol_errorstr to display a diagnostic.
enum term_color_error_t {
    TermColorErrorNone = 0,
    TermColorErrorAllocationFailed = 1,
    TermColorErrorPrintingFailed = 2,
    TermColorErrorInvalidColor = 3,
    TermColorErrorUnterminatedColor = 4,
    TERM_COLOR_ERROR_COUNT
};

// USE WITH CAUTION
int _tcol_color_generate(char* dst, size_t dstn, size_t* len, int rep,
                         int foreground, int background);

// Returns a human-readable string describing the given libtermcolor error.
const char* tcol_errorstr(const enum term_color_error_t err);

// By default libtermcolor will use color. However this behavior can be manually
// overridden using this function.
void tcol_override_color_checks(bool enable_color);

// Printfs the colorized format string to the specified stream.
int tcol_fprintf(FILE* stream, const char* fmt, ...);

// Printfs the colorized format string to the standard output.
int tcol_printf(const char* fmt, ...);


// parses a termcolor color and puts result in dest
int tcol_color_parse(char* dst, size_t dstn, char color[16],
                     size_t k, size_t* len);
#endif /* _LIBTERMCOLOR_TERMCOLOR_H */
