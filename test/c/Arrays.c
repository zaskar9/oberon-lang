//
// Created by Michael Grossniklaus on 12/21/20.
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

const int SIZE = 5000;
const int MAXVAL = 10 * SIZE;

int a[5000];

struct Array {
    int dim;
    int vec[];
};

void print(int value[]) {
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", value[i]);
    }
}

void destroy(int value[]) {
    for (int i = 0; i < SIZE; i++) {
        value[i] = i;
    }
}

void Len(struct Array *arr) {
    arr->dim = 100;
    arr->vec[99] = 20;
}

int main(int argc, const char* argv[]) {
    // int a[SIZE];
    srand(time(NULL));
    for (int i = 0; i < SIZE; i++) {
        a[i] = rand() % 100 + 1;
    }
    print(a);
    destroy(a);
    print(a);
    return 0;
}
