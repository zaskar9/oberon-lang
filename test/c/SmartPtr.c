//
// Created by Michael Grossniklaus on 3/11/22.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct smart_ptr {
    uintptr_t ref;
    int cnt;
};

int main(int argc, const char* argv[]) {
    struct smart_ptr ptr;
    float val = 100;
    ptr.ref = (uintptr_t) &val;
    ptr.cnt = 0;
    float *pval = (float*) ptr.ref;
    printf("%f\n", (float) *pval);
}
