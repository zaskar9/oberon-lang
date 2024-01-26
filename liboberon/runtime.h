//
// Created by Michael Grossniklaus on 10/21/22.
//

#include <time.h>

float rt_realf(int x);
int rt_entierf(float x);

int rt_timespec_get(struct timespec* const time_spec, int const base);

void rt_out_int(long i, int n);