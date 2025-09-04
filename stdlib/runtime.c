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

bool olang_files_fexists(const char *name) {
#if (defined(_WIN32) || defined(_WIN64)) && !(defined(__MINGW32__) || defined(__MINGW64__))
    return _access(name, 0) == 0;
#else
    return access(name, F_OK) == 0;
#endif
}

void olang_files_fregister(FILE *handle, const char *name) {
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
}

int64_t olang_files_flength(FILE *file) {
    int err = fseek(file, 0, SEEK_END);
    if (!err) {
        return ftell(file);
    }
    return -1;
}

bool olang_files_fseek(FILE *file, const int64_t offset) {
    return fseek(file, offset, SEEK_SET) == 0;
}

void olang_files_fdate(const char *name, int64_t *t, int64_t *d) {
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

bool olang_in_getchar(char *ch) {
    const int n = getchar();
    *ch = (char) n;
    return n != EOF;
}

bool olang_in_ungetchar(const char ch) {
    return ungetc(ch, stdin) == ch;
}

bool olang_in_getfloat(float *f) {
    return scanf("%f", f) == 1;
}

bool olang_in_getdouble(double *d) {
    return scanf("%lf", d) == 1;
}

bool olang_in_getint(const char *buf, int32_t *val, bool hex) {
    if (hex) {
        return sscanf(buf, "%x", (uint32_t *) val) == 1;
    }
    return sscanf(buf, "%d", val) == 1;
}

bool olang_in_getlong(const char *buf, int64_t *val, bool hex) {
    if (hex) {
        return sscanf(buf, "%lx", (uint64_t *) val) == 1;
    }
    return sscanf(buf, "%ld", val) == 1;
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
