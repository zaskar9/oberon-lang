//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "runtime.h"

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ieee754.h"

float rt_realf(const int32_t x) {
    return (float) x;
}

int32_t rt_entierf(const float x) {
    return (int32_t) floorf(x);
}

int32_t rt_timespec_get(struct timespec* const time_spec, const void *time_spec_td, int32_t const base) {
    (void) time_spec_td;
    return timespec_get(time_spec, base);
}

int32_t rt_reals_expo(const float x) {
    const union ieee754_float f = { .f = x };
    return f.ieee.exponent;
}

int32_t rt_reals_expoL(const double x) {
    const union ieee754_double d = { .d = x };
    return d.ieee.exponent;
}

float rt_reals_ten(const int32_t e) {
    return powf(10.0, (float) e);
}

double rt_reals_tenL(const int32_t e) {
    return pow(10.0, e);
}

void rt_reals_convert(const float x, const int32_t n, char* d) {
    int i = (int) floorf(x);
    int k = 0;
    while (k < n) {
        d[k] = (char)(i % 10 + 0x30);
        i = i / 10;
        ++k;
    }
}

int32_t rt_reals_nan_code(const float x) {
    const union ieee754_float f = { .f = x };
    if (f.ieee.exponent == 255) {
        return f.ieee.mantissa;
    }
    return -1;
}

void rt_reals_nan_codeL(const double x, int32_t *l, int32_t *h) {
    const union ieee754_double d = { .d = x };
    if (d.ieee.exponent == 2047) {
        *l = (int32_t)d.ieee.mantissa1;
        *h = (int32_t)d.ieee.mantissa0;
    } else {
        *l = -1;
        *h = -1;
    }
}

float rt_reals_nan(void) {
    const union ieee754_float f = { .ieee.exponent = 255, .ieee.mantissa = 0x3fffff };
    return f.f;
}

double rt_reals_nanL(void) {
    const union ieee754_double d = { .ieee.exponent = 2047, .ieee.mantissa0 = 0xfffff, .ieee.mantissa1 = 0xffffffff };
    return d.d;
}
