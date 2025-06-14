#include <stdio.h>
#include <stdlib.h>


#define OBNC_DIV(x, y) (((x) >= 0)? (x) / (y): ((x) - OBNC_MOD(x, y)) / (y))

#define OBNC_MOD(x, y) (((x) >= 0)? (x) % (y): (((x) % (y)) + (y)) % (y))

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

void WriteNum(int64_t x, char buf[]) {
    int i = 0;
    while ((x < -64) || (x > 63)) {
        buf[i] = (char) (OBNC_MOD(x, 128) + 128);
        x = OBNC_DIV(x, 128);
        i++;
    }
    buf[i] = (char) OBNC_MOD(x, 128);
}

void ReadNum(int64_t *x, char buf[]) {
    int64_t n = 0;
    int64_t s = 0;
    int y, z;
    int i = 0;
    uint8_t ch = buf[i];
    while (ch >= 128) {
        n += (ch - 128) << s;
        s += 7;
        i++;
        ch = buf[i];
    }
    y = OBNC_MOD(ch, 64) - OBNC_DIV(ch, 64) * 64;
    if (y < 0) {
        z = -((-y) << s);
    } else {
        z = y << s;
    }
    printf("(%lld, %d)\n", n, z);
    *x = n + z;
}


int main(int argc, char* argv[]) {
    // printf("(%d, %d)\n", DIV1(5, 3), MOD1(5, 3));
    // printf("(%d, %d)\n", DIV2(5, 3), MOD2(5, 3));
    // printf("(%d, %d)\n", DIV1(-5, 3), MOD1(-5, 3));
    // printf("(%d, %d)\n", DIV2(-5, 3), MOD2(-5, 3));
    /*
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
    */
    char buf[8];
    WriteNum(2147483647, buf);
    for (int i = 0; i < 8; ++i) {
        printf("%d\n", buf[i]);
    }
    int64_t num;
    ReadNum(&num, buf);
    printf("%lld\n", num);
}