//
// Created by Michael Grossniklaus on 2025-05-27.
//

#include <stdio.h>
#include <stdlib.h>

int cmp(int16_t x, uint8_t y) {
    return x > y;
}

int main(int argc, char* argv[]) {
    printf("%d\n", cmp(atoi(argv[1]), atoi(argv[2])));
    return 0;
}
