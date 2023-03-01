//
// Created by Michael Grossniklaus on 10/20/22.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, const char* argv[]) {
    int i1 = 1;
    int i2 = 2;
    long l1 = i1;
    i2 = (int) l1;
    double d1 = 3.141592654;
    float f1 = (float) d1;
    printf("%f\n", f1);
    f1 = i1;

    float pi = 3.141592654;
    printf("%f\n", pi);
}
