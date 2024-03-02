#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../liboberon/ieee754.h"

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

int main(void) {
    float e = 2.7182818284;
    float pi = 3.1415926535;
    int i1 = 1;
    int i2 = 2;
    float f1 = pi; rt_out_real(f1, 16); printf("\n");
    f1 = i1; rt_out_real(f1, 16); printf("\n");
    float f2 = i1 + i2; rt_out_real(f2, 16); printf("\n");
    f2 = i1 - f2; rt_out_real(f2, 16); printf("\n");
    f2 = f1 * i2; rt_out_real(f2, 16); printf("\n");
    f2 = f1 / f2; rt_out_real(f2, 16); printf("\n");
    rt_out_real(sqrtf(2), 16); printf("\n");
    rt_out_real(expf(12.0), 16); printf("\n");
    rt_out_real(logf(e), 16); printf("\n");
    rt_out_real(sinf(pi), 16); printf("\n");
    rt_out_real(cosf(pi), 16); printf("\n");
    rt_out_real(atan(pi), 16); printf("\n");
    printf("3\n");
    rt_out_real(3, 16); printf("\n");
    f1 = 0.1;
    f2 = 0.2;
    rt_out_real(f1 + f2, 16); printf("\n");
    rt_out_real(9 / 5, 16); printf("\n");
    return 0;
}