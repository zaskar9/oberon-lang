#include <stdio.h>
#include <stdlib.h>

typedef int Vector[10];

struct Array {
    int dim;
    Vector vec;
};

struct PersonDesc {
    char* name;
    char* title[2];
    struct Array address;
    int zipcode;
};

int Len(struct Array *a) {
    int i = a->dim;
    return i;
}

void update(int *v) {
    *v = 10;
}

struct PersonDesc p;

int main(void) {
    p.zipcode = 8280;
    p.address.dim = 10;
    p.address.vec[9] = 42;
    // printf("%d\n", Len(&p.address));
    return 0;
}