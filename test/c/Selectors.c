#include <stdio.h>

struct Array {
    int dim;
    int vec[10];
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

int main(void) {
    struct PersonDesc p;
    p.zipcode = 8280;
    p.address.dim = 10;
    p.address.vec[3] = 42;
    printf("%d\n", Len(&p.address));
    return 0;
}