//
// Created by Michael Grossniklaus on 10/21/22.
//

#ifndef _STDLIB_RUNTIME_H

#define _STDLIB_RUNTIME_H 1

#include <stdint.h>
#include <time.h>

float rt_realf(int32_t x);
int32_t rt_entierf(float x);

int32_t rt_timespec_get(struct timespec *time_spec, const void *time_spec_td, int32_t base);

int32_t rt_reals_expo(float x);
int32_t rt_reals_expoL(double x);

float rt_reals_ten(int32_t e);
double rt_reals_tenL(int32_t e);

void rt_reals_convert(float x, int32_t n, char* d);

int32_t rt_reals_nan_code(float x);
void rt_reals_nan_codeL(double x, int32_t *l, int32_t *h);

float rt_reals_nan(void);
double rt_reals_nanL(void);

#endif //_STDLIB_RUNTIME_H
