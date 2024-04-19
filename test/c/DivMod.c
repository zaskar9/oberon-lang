#include <stdio.h>
#include <stdlib.h>

static int MOD1(int a, int b) {
    return ( a % b + b ) % b;
}

static int DIV1(int a, int b) {
    return (a - MOD1(a, b)) / b;
}

static int DIV2(int a, int b) {
    return (a < 0) ? (a - b + 1) / b : a / b;
}

static int MOD2(int a, int b) {
    return (a < 0) ? (b - 1) + ((a - b + 1)) % b : a % b;
}

int64_t floor_div(int64_t a, int64_t b) {
    int64_t d = a / b;
    int64_t r = a % b;
    return r ? (d - ((a < 0) ^ (b < 0))) : d;
}

int64_t floor_div2(int64_t a, int64_t b) {
    return a / b - (a % b != 0) * ((a < 0) ^ (b < 0));
}

int64_t euclidean_mod(int64_t a, int64_t b) {
    int64_t r = a % b;
    r += b & (-(r < 0));
    return r;
}

int64_t euclidean_mod2(int64_t a, int64_t b) {
    int64_t r = a % b;
    r += (r >> (8 * sizeof(int64_t) - 1)) & b;
    return r;
}

int main(int argc, char* argv[]) {
    // printf("(%d, %d)\n", DIV1(5, 3), MOD1(5, 3));
    // printf("(%d, %d)\n", DIV2(5, 3), MOD2(5, 3));
    // printf("(%d, %d)\n", DIV1(-5, 3), MOD1(-5, 3));
    // printf("(%d, %d)\n", DIV2(-5, 3), MOD2(-5, 3));
    if (argc > 2) {
        int x = atoi(argv[1]);
        int y = atoi(argv[2]);
        printf("(%d, %d)\n", DIV1(x, y), MOD1(x, y));
        printf("(%d, %d)\n", DIV2(x, y), MOD2(x, y));
        printf("(%lld, %lld)\n", floor_div(x, y), euclidean_mod(x, y));
    }
    // div_t res = div(-5, 3);
    // printf("(%d, %d)\n", res.quot, res.rem);
    printf("(%lld, %lld)\n", floor_div2(-5, 3), euclidean_mod2(-5, 3));
}