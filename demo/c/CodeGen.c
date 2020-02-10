//
// Created by Michael Grossniklaus on 2/10/20.
//

#include <stdlib.h>
#include <stdio.h>


int max(int x, int y) {
    int res;
    if (x > y) {
        res = x;
    } else {
        res = y;
    }
    return res;
}

int main(int argc, const char* argv[]) {
    printf("%d\n", max(12, 9));
    return 0;
}
