//
// Created by Michael Grossniklaus on 10/21/22.
//

#include <stdint.h>
#include <time.h>

float rt_realf(int32_t x);
int32_t rt_entierf(float x);

int32_t rt_timespec_get(struct timespec* const time_spec, int32_t const base);

int32_t rt_reals_expo(float x);
int32_t rt_reals_expoL(double x);

float rt_reals_ten(int32_t e);
double rt_reals_tenL(int32_t e);

int32_t rt_reals_nan_code(float x);
void rt_reals_nan_codeL(double x, int32_t *l, int32_t *h);

float rt_reals_nan();
double rt_reals_nanL();