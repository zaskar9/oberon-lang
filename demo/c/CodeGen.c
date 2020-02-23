//
// Created by Michael Grossniklaus on 2/10/20.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int a[20];
int x, y;
struct Point2D {
    int x, y;
};

void test(struct Point2D point) {
    printf("(%d, %d)", point.x, point.y);
}

void check(int i) {
    bool b = (i >= 0 && i < 10) || (i >= 90 && i < 100);
    if (b == true) {
        printf("passed\n");
    }
}

int main(int argc, const char* argv[]) {
    struct Point2D point;
    point.x = 9;
    point.y = 27;
    test(point);
    return 0;
}
