//
// Created by Michael Grossniklaus on 4/5/25.
//

#include <strings.h>

int str_cmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int str_cmp2(const char* s1, const char* s2) {
    return strcmp(s1, s2);
}