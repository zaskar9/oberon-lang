//
// Created by Michael Grossniklaus on 9/16/22.
//

#include <math.h>
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
