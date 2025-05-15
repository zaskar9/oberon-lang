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
bool olang_files_file_exists(const char *, const void *);
FILE *olang_files_file_open(const char *, const void *);
int32_t olang_files_file_register(FILE *, const char *, const void *);
bool olang_files_file_remove(const char *, const void *);
bool olang_files_file_rename(const char *, const void *, const char *, const void*);
int64_t olang_files_file_length(FILE *);
bool olang_files_file_seek(FILE*, const int64_t);

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
