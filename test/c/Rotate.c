//
// Created by Michael Grossniklaus on 2/24/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define LONG_WIDTH (sizeof (long) * CHAR_BIT)

long leftRotate(unsigned long x, long n) {
    return (x << n) | (x >> (LONG_WIDTH - n));
}

long rightRotate(unsigned long x, long n) {
    return (x >> n) | (x << (LONG_WIDTH - n));
}

static inline unsigned long rotl64 (unsigned long n, unsigned long c) {
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);  // assumes width is a power of 2.

    // assert ( (c<=mask) &&"rotate by type width or more");
    c &= mask;
    return (n << c) | (n >> ((-c) & mask));
}

static inline unsigned long rotr64(unsigned long n, unsigned long c) {
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

    // assert ( (c<=mask) &&"rotate by type width or more");
    c &= mask;
    return (n >> c) | (n << ((-c) & mask));
}

int main(int argc, const char* argv[]) {
    long x = -32;
    long n = 2;
    printf("%lu\n", rotl64(x, n));
    printf("%lu\n", rotr64(x, n));
    abort();
}