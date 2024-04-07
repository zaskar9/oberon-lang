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

float rt_realf(int32_t x) {
    return (float) x;
}

int32_t rt_entierf(float x) {
    return (int32_t) floorf(x);
}

int32_t rt_timespec_get(struct timespec* const time_spec, int32_t const base) {
    return (int32_t) timespec_get(time_spec, base);
}

int32_t rt_reals_expo(float x) {
    union ieee754_float f = { .f = x };
    return f.ieee.exponent;
}

int32_t rt_reals_expoL(double x) {
    union ieee754_double d = { .d = x };
    return d.ieee.exponent;
}

float rt_reals_ten(int32_t e) {
    return powf(10.0, e);
}

double rt_reals_tenL(int32_t e) {
    return pow(10.0, e);
}

void rt_reals_convert(float x, int32_t n, char* d) {
    int i = (int) floorf(x);
    int k = 0;
    while (k < n) {
        d[k] = (char)(i % 10 + 0x30);
        i = i / 10;
        ++k;
    }
}

int32_t rt_reals_nan_code(float x) {
    union ieee754_float f = { .f = x };
    if (f.ieee.exponent == 255) {
        return f.ieee.mantissa;
    }
    return -1;
}

void rt_reals_nan_codeL(double x, int32_t *l, int32_t *h) {
    union ieee754_double d = { .d = x };
    if (d.ieee.exponent == 2047) {
        *l = d.ieee.mantissa1;
        *h = d.ieee.mantissa0;
    } else {
        *l = -1;
        *h = -1;
    }
}

float rt_reals_nan() {
    union ieee754_float f = { .ieee.exponent = 255, .ieee.mantissa = -1 };
    return f.f;
}

double rt_reals_nanL() {
    union ieee754_double d = { .ieee.exponent = 2047, .ieee.mantissa0 = -1, .ieee.mantissa1 = -1 };
    return d.d;
}
