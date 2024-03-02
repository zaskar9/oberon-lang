//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "runtime.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ieee754.h"

float rt_realf(int x) {
    return (float) x;
}

int rt_entierf(float x) {
    return (int) floorf(x);
}

int rt_timespec_get(struct timespec* const time_spec, int const base) {
    return timespec_get(time_spec, base);
}

void rt_out_int(long i, int n) {
    if (n < 0) {
        n = 0;
    }
    char buf[21]; // 64-bit is maximally 20 digits (with sign), long plus '\0'
    sprintf(buf, "%ld", i);
    int len = strlen(buf);
    if (len < n) {
        char *out = (char*) malloc((n + 1) * sizeof(char));
        int diff = n - len;
        memmove(out + diff, buf, len + 1);
        memset(out, ' ', diff);
        printf("%s", out);
    } else {
        printf("%s", buf);
    }
}

void rt_out_real(float x, int n) {
    int e = rt_reals_expo(x);
    if (e == 0) {
        printf(" 0");
        do {
            putchar(' ');
            --n;
        } while (n > 4);
    } else if (e == 255) {
        printf(" NaN");
        while (n > 4) {
            putchar(' ');
            --n;
        }
    } else {
        if (n <= 9) {
            n = 3;
        } else {
            n -= 6;
        }
        do {
            putchar(' ');
            --n;
        } while (n > 9);
        // there are 2 < n <= 8 digits to be written
        if (x < 0.0) {
            putchar('-');
            x = -x;
        } else {
            putchar(' ');
        }
        e = (e - 127) * 77 / 256;
        if (e >= 0) {
            x = x / rt_reals_ten(e);
        } else {
            x = rt_reals_ten(-e) * x;
        }
        if (x > 10.0) {
            x = 0.1 * x;
            ++e;
        }
        float x0 = rt_reals_ten(n - 1);
        x = x0 * x + 0.5;
        if (x >= 10.0 * x0) {
            x = x * 0.1;
            ++e;
        }
        char d[9];
        rt_reals_convert(x, n, d);
        --n;
        putchar(d[n]);
        putchar('.');
        do {
            --n;
            putchar(d[n]);
        } while (n > 0);
        putchar('E');
        if (e < 0) {
            putchar('-');
            e = -e;
        } else {
            putchar('+');
        }
        putchar(e / 10 + 0x30);
        putchar(e % 10 + 0x30);
    }
}

int rt_reals_expo(float x) {
    union ieee754_float f = { .f = x };
    return f.ieee.exponent;
}

int rt_reals_expoL(double x) {
    union ieee754_double d = { .d = x };
    return d.ieee.exponent;
}

float rt_reals_ten(int e) {
    return powf(10.0, e);
}

double rt_reals_tenL(int e) {
    return pow(10.0, e);
}

void rt_reals_convert(float x, int n, char* d) {
    int i = (int) floorf(x);
    int k = 0;
    while (k < n) {
        d[k] = (char)(i % 10 + 0x30);
        i = i / 10;
        ++k;
    }
}
