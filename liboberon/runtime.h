//
// Created by Michael Grossniklaus on 10/21/22.
//

#include <time.h>

float rt_realf(int x);
int rt_entierf(float x);

int rt_timespec_get(struct timespec* const time_spec, int const base);

void rt_out_int(long i, int n);
void rt_out_real(float x, int n);

int rt_reals_expo(float x);
int rt_reals_expoL(double x);
float rt_reals_ten(int e);
double rt_reals_tenL(int e);
void rt_reals_convert(float x, int n, char* d);