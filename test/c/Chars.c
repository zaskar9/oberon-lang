//
// Created by Michael Grossniklaus on 3/2/23.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef char String[265];

typedef struct {
    long len;
    char str[];
} TString;

String string, s3;
char *s4, *s5;

const TString hw = { .len = 13, .str = "Hello World!" };
const TString hn = { .len = 13, .str = "Hoi Niklaus!" };
TString s1;

void printStr(const char* str, int len) {
    int i = 0;
    while (i < len && str[i] != '\0') {
        printf("%c", str[i++]);
    }
}

int main(int argc, const char** argv) {
//
//    string[0] = 'O';
//    string[1] = 'b';
//    string[2] = 'e';
//    string[3] = 'r';
//    string[4] = 'o';
//    string[5] = 'n';
//    string[6] = '!';
//    string[7] = '\0';
//
//    printf("s1: %s\n", string);
//
//
//    strcpy(s3, "Hello World!");
//    printf("s3: %s\n", s3);
//
//    s4 = (char*) malloc(100 * sizeof(char));
//    strcpy(s4, "Hello World!");
//    printf("s4: %s\n", s4);
//    free(s4);
//
//    char ch = (char)'A';
//    outChar(ch);
//
//    char s1[] = "Hello";

    s4 = (char*) hw.str;
    s5 = (char*) hn.str;
    printf("s4: %s\n", s4);
    printf("s5: %s\n", s5);
    printStr(hw.str, 100);
    printf("\n");

//    s1 = hw;
//    printf("s1: %s\n", s1.str);

    exit(0);
}
