//
// Created by Michael Grossniklaus on 3/17/20.
//

#include <stdlib.h>
#include <stdio.h>

struct TParent { int x, y, z; };
struct TRecord { int a, b, c; struct TParent parent; };

void Proc1(struct TRecord param, struct TParent parent) {
    param.a = 1; param.b = 2; param.c = 3;
    param.parent = parent;
}

int main(int argc, const char* argv[]) {
    struct TRecord param;
    struct TParent parent = { 4, 5, 6 };
    Proc1(param, parent);
    printf("(%d,%d,%d)[%d,%d,%d]\n", param.a, param.b, param.c, param.parent.x, param.parent.y, param.parent.z);
    return 0;
}