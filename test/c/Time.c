//
// Created by Michael Grossniklaus on 3/22/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct my_timespec {
    long secs;
    long nsecs;
};

static long time_millis(void) {
    struct my_timespec ts;
    timespec_get((struct timespec*) &ts, TIME_UTC);
    printf("[%ld][%ld]\n", ts.secs, ts.nsecs);
    return (long)ts.secs * 1000 + ts.nsecs / 1000000;
}

int main(int argc, const char* argv[]) {
    printf("%lu\n", time_millis());
    exit(0);
}