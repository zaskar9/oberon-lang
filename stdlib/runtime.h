//
// Created by Michael Grossniklaus on 10/21/22.
//

#ifndef _STDLIB_RUNTIME_H

#define _STDLIB_RUNTIME_H 1

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

// Module `Oberon`
int32_t olang_oberon_timespec_get(struct timespec *, const void *, int32_t);

// Module `Files`
bool olang_files_fexists(const char *);
void olang_files_file_register(FILE *, const char *);
int64_t olang_files_flength(FILE *);
bool olang_files_fseek(FILE*, int64_t);
void olang_files_fdate(const char *, int64_t *, int64_t *);

// Module `Math`
float olang_math_realf(int32_t);
int32_t olang_math_entierf(float);

// Module `Reals`
int32_t olang_reals_expo(float);
int32_t olang_reals_expoL(double);
float olang_reals_ten(int32_t);
double olang_reals_tenL(int32_t);
int32_t olang_reals_nan_code(float);
void olang_reals_nan_codeL(double, int32_t *, int32_t *);
float olang_reals_nan(void);
double olang_reals_nanL(void);


#endif //_STDLIB_RUNTIME_H
