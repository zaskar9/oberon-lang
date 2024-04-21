//
// Created by Michael Grossniklaus on 12/21/20.
//

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

const int SIZE = 5000;
const int MAXVAL = 10 * SIZE;

int a[5000];

typedef struct {
    int dim;
    float vec[10];
    int test;
} Array;

//void print(int value[]) {
//    for (int i = 0; i < SIZE; i++) {
//        printf("%d ", value[i]);
//    }
//}

//void destroy(int value[]) {
//    for (int i = 0; i < SIZE; i++) {
//        value[i] = i;
//    }
//}

void test(Array *rec) {
    int test = *((int*)(((char*)rec) + (offsetof(Array, test) - offsetof(Array, dim))));
    printf("%d\n", test);
}

int main(int argc, const char* argv[]) {
    Array m[10];
    Array s;
    s.dim = 10;
    s.test = 20;
    m[5] = s;

    int len = *((int*)(((char*)m[5].vec) + (offsetof(Array, dim) - offsetof(Array, vec))));
    printf("%d\n", len);
    test(&s);
    return 0;
}
