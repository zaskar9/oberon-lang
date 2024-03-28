//
// Created by Michael Grossniklaus on 10/21/22.
//

#include <stdint.h>
#include <time.h>

float rt_realf(int32_t x);
int32_t rt_entierf(float x);

int32_t rt_timespec_get(struct timespec* const time_spec, int32_t const base);

void rt_out_int(int64_t i, int32_t n);
void rt_out_real(float x, int32_t n);

int32_t rt_reals_expo(float x);
int32_t rt_reals_expoL(double x);
float rt_reals_ten(int32_t e);
double rt_reals_tenL(int32_t e);
void rt_reals_convert(float x, int32_t n, char* d);