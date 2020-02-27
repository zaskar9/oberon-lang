//
// Created by Michael Grossniklaus on 2/10/20.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct Point2D {
    int x, y;
};
struct Point3D {
    struct Point2D p2d;
    int z;
};
struct ColorRGB {
    int r, g, b;
};
struct Polygon3D {
    struct Point3D points[20];
    struct ColorRGB color;
};

int x, y, z, i;
int a[20];
struct Point2D p2d;
struct Point3D p3d;
struct Point3D points[20];
struct Polygon3D poly3d;

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
