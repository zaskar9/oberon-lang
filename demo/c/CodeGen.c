//
// Created by Michael Grossniklaus on 2/10/20.
//

#include <stdlib.h>
#include <stdio.h>

int a[20];
int x, y;
struct Point2D {
    int x, y;
};

void test(struct Point2D point) {
    printf("(%d, %d)", point.x, point.y);
}

int main(int argc, const char* argv[]) {
    struct Point2D point;
    point.x = 9;
    point.y = 27;
    test(point);
    return 0;
}
