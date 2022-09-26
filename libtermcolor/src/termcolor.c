// libtermcolor: termcolor.c
// Copyright (C) 2021 Ethan Uppal. All rights reserved.

#include "termcolor.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

static bool use_color = true;

const char* tcol_errorstrs[TERM_COLOR_ERROR_COUNT] = {
    "Success",
    "Virtual memory exhausted",
    "Printing failed",
    "Invalid color provided",
    "Color was not terminated with '}'"
};

inline const char* tcol_errorstr(const enum term_color_error_t err) {
    return tcol_errorstrs[err];
}

void tcol_override_color_checks(bool enable_color) {
    use_color = enable_color;
}

int _termcolor_internal_lookup(const char color_name) {
    switch (color_name) {
        case 'N': return 30;
        case 'R': return 31;
        case 'G': return 32;
        case 'Y': return 33;
        case 'B': return 34;
        case 'M': return 35;
        case 'C': return 36;
        case 'W': return 37;
        default: return -1;
    }
}

enum _termcolor_internal_color {
    _termcolor_internal_color_BOLD = 1 << 1,
    _termcolor_internal_color_FANT = 1 << 2,
    _termcolor_internal_color_UNDR = 1 << 3,
    _termcolor_internal_color_BLNK = 1 << 4,
    _termcolor_internal_color_STRK = 1 << 5,
    _termcolor_internal_color_ITLC = 1 << 6
};

int _tcol_color_generate(char* dst, size_t dstn, size_t* len, int rep,
                         int foreground, int background) {
    // Much of the code here was informed by the following gist:
    // https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

    size_t j = 0;

    // This macro checks to make sure we don't overrun the buffer when we add
    // a new character,
    #define __APPEND(c) do { \
        dst[j++] = (c); \
        if (j >= dstn) return TermColorErrorNone; \
    } while (0)

    // Every escape code begans with "\e["
    __APPEND('\033');
    __APPEND('[');

    // Add the appropriate parameters to our escape sequence depending on the
    // flags set in rep(resentation).
    if (rep & _termcolor_internal_color_BOLD) {
        __APPEND('1');
        __APPEND(';');
    }
    if (rep & _termcolor_internal_color_FANT) {
        __APPEND('2');
        __APPEND(';');
    }
    if (rep & _termcolor_internal_color_ITLC) {
        __APPEND('3');
        __APPEND(';');
    }
    if (rep & _termcolor_internal_color_UNDR) {
        __APPEND('4');
        __APPEND(';');
    }
    if (rep & _termcolor_internal_color_BLNK) {
        __APPEND('5');
        __APPEND(';');
    }
    if (rep & _termcolor_internal_color_STRK) {
        __APPEND('9');
        __APPEND(';');
    }

    // If foreground and background colors were provided, snprintf them to dst.
    if (foreground != -1) {
        j += snprintf(dst + j, dstn - j, "%d;", foreground);
        if (j >= dstn) return TermColorErrorNone;
    }
    if (background != -1) {
        j += snprintf(dst + j, dstn - j, "%d;", background + 10);
        if (j >= dstn) return TermColorErrorNone;
    }

    if (dst[j - 1] == ';') {
        j--;
    }

    // Graphics mode escape sequences end with 'm'
    __APPEND('m');

    *len = j;
    return TermColorErrorNone;
}

int tcol_color_parse(char* dst, size_t dstn, char color[16], size_t k,
                     size_t* len) {
    if (!use_color) {
        *len = 0;
        return TermColorErrorNone;
    }

    // '0' signifies no color, i.e. a reset
    if (*color == '0' && k == 1) {
        size_t j = 0;
        __APPEND('\033');
        __APPEND('[');
        __APPEND('m');
        *len = j;
        return TermColorErrorNone;
    }

    // As -1 they are ignored. Otherwise they are set to the foreground color
    // code. The backround code is later added 10 to.
    int foreground = -1;
    int background = -1;

    enum _termcolor_internal_color rep = 0;
    for (size_t i = 0; i < k; i++) {
        switch (color[i]) {
            // Farily straightforward; just sets the flags corresponding to the
            // color attributes.
            case '+': rep |= _termcolor_internal_color_BOLD; break;
            case '-': rep |= _termcolor_internal_color_FANT; break;
            case '_': rep |= _termcolor_internal_color_UNDR; break;
            case '*': rep |= _termcolor_internal_color_BLNK; break;
            case '~': rep |= _termcolor_internal_color_STRK; break;
            case '/': rep |= _termcolor_internal_color_ITLC; break;

            // Here we take the character after the ',' and use that as our
            // background color code, like Nano does.
            case ',': {
                if (i + 1 >= k) {
                    return TermColorErrorInvalidColor;
                }
                const int result = _termcolor_internal_lookup(color[i + 1]);
                if (result == -1) {
                    return TermColorErrorInvalidColor;
                }
                background = result;
                i++;
                break;
            }
            default: {
                // If we haven't matched anything, we assume its a foreground
                // color. Make sure it is and error if not.
                const int result = _termcolor_internal_lookup(color[i]);
                if (result != -1) {
                    foreground = result;
                    break;
                }
                return TermColorErrorInvalidColor;
            }
        }
    }

    return _tcol_color_generate(dst, dstn, len, rep, foreground, background);
}

static inline int tcol_fmt_parse(char* dst, size_t dstn, const char* src,
                                 size_t srcn) {
    // Variables:
    //   i = index in source
    //   j = index in destination

    // 1. We loop throughout the source until we reach the end or until we
    // cannot write into dst anymore.
    if (dstn == 0) {
        return TermColorErrorNone;
    }
    size_t j = 0;
    dstn--;
    for (size_t i = 0; i < srcn && j < dstn; i++) {
        loop_top:
        // 2. We check if the current character is '{'.
        if (src[i] == '{') {
            // 3.B. If it is, we make sure it's not escaped with "{{".
            if (i + 1 < srcn && src[i + 1] == '{') {
                i++;
                dst[j++] = '{';
            } else {
                i++;

                // 4. If not escape, we obtain the color and parse it.
                char color[16];
                int k = 0;
                while (i < srcn && src[i] != '}') {
                    // Allow spaces in the color
                    if (src[i] == ' ') {
                        i++;
                        if (src[i] == '}') {
                            i++;
                            // If the color is all spaces, ignore it.
                            goto loop_top;
                        }
                        continue;
                    }

                    // Copy over the contents of the source to the color buffer.
                    color[k++] = src[i++];

                    // Make sure no bounds are overrun.
                    if (k > 16) {
                        return TermColorErrorUnterminatedColor;
                    }
                }

                // Confirm we ended on a right bracket
                if (src[i] != '}') {
                    return TermColorErrorUnterminatedColor;
                }

                // Create the escape sequence.
                size_t len;
                int status = tcol_color_parse(dst + j, dstn - j, color, k,
                                              &len);
                if (status != TermColorErrorNone) {
                    return status;
                }
                j += len;
            }
        } else {
            // 3.A. If not, just write the character to the destination to
            // copy verbatim.
            dst[j++] = src[i];
        }
    }
    dst[dstn] = 0;
    dst[j] = 0;
    return TermColorErrorNone;
}

static int tcol_vfprintf(FILE* stream, const char* fmt, va_list ap) {
    // Gets the length of the format string and calculates a length for the new
    // format string to be created.
    const size_t l = strlen(fmt);
    const size_t n = l * 2 + 16;

    // Allocates and produces the new format string.
    char* buffer = malloc(n);
    if (buffer == NULL) {
        return TermColorErrorAllocationFailed;
    }
    const int status = tcol_fmt_parse(buffer, n, fmt, l);
    if (status != TermColorErrorNone) {
        free(buffer);
        return status;
    }

    // printf("%s\n", buffer);

    // Perform the printf itself.
    if (vfprintf(stream, buffer, ap) < 0) {
        free(buffer);
        return TermColorErrorPrintingFailed;
    }

    // Cleanup resources.
    free(buffer);

    return TermColorErrorNone;
}

// These functions just create a variable argument list and call tcol_vfprintf
// with the appropriate FILE* stream.
int tcol_fprintf(FILE* stream, const char* fmt, ...) {
      va_list ap;
      va_start(ap, fmt);
      const int status = tcol_vfprintf(stream, fmt, ap);
      va_end(ap);
      return status;
}
int tcol_printf(const char* fmt, ...) {
      va_list ap;
      va_start(ap, fmt);
      const int status = tcol_vfprintf(stdout, fmt, ap);
      va_end(ap);
      return status;
}
