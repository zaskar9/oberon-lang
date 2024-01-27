//
// Created by Michael Grossniklaus on 9/16/22.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "runtime.h"

float rt_realf(int x) {
    return (float) x;
}

int rt_entierf(float x) {
    return (int) floorf(x);
}

int rt_timespec_get(struct timespec* const time_spec, int const base) {
    return timespec_get(time_spec, base);
}

void rt_out_int(long i, int n) {
    if (n < 0) {
        n = 0;
    }
    char buf[21]; // 64-bit is maximally 20 digits (with sign), long plus '\0'
    sprintf(buf, "%ld", i);
    int len = strlen(buf);
    if (len < n) {
        char *out = (char*) malloc((n + 1) * sizeof(char));
        int diff = n - len;
        memmove(out + diff, buf, len + 1);
        memset(out, ' ', diff);
        printf("%s", out);
    } else {
        printf("%s", buf);
    }
}
