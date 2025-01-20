//
// Created by Michael Grossniklaus on 1/19/25.
//

#include <stdio.h>
#include <stdlib.h>

void test(int a) {
    int b = -a;
    printf("%d\n", b);
}

int main(void) {
    int a = -2147483648;
    test(a);
    exit(0);
}
