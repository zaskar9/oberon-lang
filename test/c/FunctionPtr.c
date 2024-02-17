#include <stdio.h>
#include <stdlib.h>

struct _Object;

typedef int (*Operator)(struct _Object this , int b);

typedef struct _Object {
    int val;
    Operator op;
} Object;

int add(struct _Object this, int b) {
    return this.val + b;
}

int multiply(struct _Object this, int b) {
    return this.val * b;
}

int main(void) {
    Object obj;
    obj.val = 5;
    obj.op = add;
    int res = obj.op(obj, 6);
    printf("Result: %d\n", res);
    return EXIT_SUCCESS;
}