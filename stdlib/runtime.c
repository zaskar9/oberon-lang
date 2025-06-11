//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "runtime.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#if (defined(_WIN32) || defined(_WIN64)) && !(defined(__MINGW32__) || defined(__MINGW64__))
  #include <io.h>
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#include "ieee754.h"

#define UNUSED(x) (void)(x)

int32_t olang_oberon_timespec_get(struct timespec* const time_spec, const void *time_spec_td, int32_t const base) {
    UNUSED(time_spec_td);
    return timespec_get(time_spec, base);
}

bool olang_files_file_exists(const char *name, const void *dv) {
    UNUSED(dv);
#if (defined(_WIN32) || defined(_WIN64)) && !(defined(__MINGW32__) || defined(__MINGW64__))
    return _access(name, 0) == 0;
#else
    return access(name, F_OK) == 0;
#endif
}

FILE *olang_files_file_open(const char *name, const void *dv) {
    UNUSED(dv);
    FILE *file = fopen(name, "r+b");
    if (file == NULL) {
        file = fopen(name, "rb");
        if (file == NULL && olang_files_file_exists(name, NULL)) {
            file = fopen(name, "ab");
        }
    }
    return file;
}

int32_t olang_files_file_register(FILE *handle, const char *name, const void *dv) {
    UNUSED(dv);
    FILE *file = fopen(name, "w+b");
    rewind(handle);
    int ch = fgetc(handle);
    while (ch != EOF) {
        ch = fputc(ch, file);
        if (ch != EOF) {
            ch = fgetc(handle);
        }
    }
    fclose(file);
    return !ferror(handle) && !ferror(file);
}

int32_t olang_files_file_remove(const char *name, const void *dv) {
    UNUSED(dv);
    return remove(name);
}

int32_t olang_files_file_rename(const char *old, const void *old_dv, const char *new, const void* new_dv) {
    UNUSED(old_dv); UNUSED(new_dv);
    return rename(old, new);
}

int64_t olang_files_file_length(FILE *file) {
    int err = fseek(file, 0, SEEK_END);
    if (!err) {
        return ftell(file);
    }
    return -1;
}

bool olang_files_file_seek(FILE *file, const int64_t offset) {
    return fseek(file, offset, SEEK_SET) == 0;
}

FILE *olang_files_file_purge(const char *name, const void *dv) {
    UNUSED(dv);
    return fopen(name, "w+b");
}

void olang_files_file_date(const char *name, const void *dv, int64_t *t, int64_t *d) {
    UNUSED(dv);
    struct stat result;
    const int error = stat(name, &result);
    if (!error) {
        const struct tm *td = localtime(&result.st_mtime);
        if (td != NULL) {
            *t = td->tm_hour << 12 | td->tm_min << 6 | td->tm_sec;
            *d = (1900 + td->tm_year) << 9 | (td->tm_mon + 1) << 5 | td->tm_mday;
        }
    }
}

float olang_math_realf(const int32_t x) {
    return (float) x;
}

int32_t olang_math_entierf(const float x) {
    return (int32_t) floorf(x);
}

int32_t olang_reals_expo(const float x) {
    const union ieee754_float f = { .f = x };
    return f.ieee.exponent;
}

int32_t olang_reals_expoL(const double x) {
    const union ieee754_double d = { .d = x };
    return d.ieee.exponent;
}

float olang_reals_ten(const int32_t e) {
    return powf(10.0, (float) e);
}

double olang_reals_tenL(const int32_t e) {
    return pow(10.0, e);
}

int32_t olang_reals_nan_code(const float x) {
    const union ieee754_float f = { .f = x };
    if (f.ieee.exponent == 255) {
        return f.ieee.mantissa;
    }
    return -1;
}

void olang_reals_nan_codeL(const double x, int32_t *l, int32_t *h) {
    const union ieee754_double d = { .d = x };
    if (d.ieee.exponent == 2047) {
        *l = (int32_t)d.ieee.mantissa1;
        *h = (int32_t)d.ieee.mantissa0;
    } else {
        *l = -1;
        *h = -1;
    }
}

float olang_reals_nan(void) {
    const union ieee754_float f = { .ieee.exponent = 255, .ieee.mantissa = 0x3fffff };
    return f.f;
}

double olang_reals_nanL(void) {
    const union ieee754_double d = { .ieee.exponent = 2047, .ieee.mantissa0 = 0xfffff, .ieee.mantissa1 = 0xffffffff };
    return d.d;
}
