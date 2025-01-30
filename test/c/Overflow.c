//
// Created by Michael Grossniklaus on 1/19/25.
//

// clang -ftrapv -S -emit-llvm Overflow.c
// clang -fsanitize=undefined -S -emit-llvm Overflow.c

#include <stdio.h>
#include <stdlib.h>

void neg(int a) {
    int b = -a;
    printf("%d\n", b);
}

void my_div(int a, int b) {
    int c = a / b;
    printf("%d\n", c);
}

void my_rem(int a, int b) {
    int c = a % b;
    printf("%d\n", c);
}

int main(void) {
    int a = -2147483648;
    neg(a);
    int b = 0;
    my_div(-a, b);
    exit(0);
}
