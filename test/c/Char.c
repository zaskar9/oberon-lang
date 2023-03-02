//
// Created by Michael Grossniklaus on 3/2/23.
//

#include <stdlib.h>
#include <stdio.h>

void outChar(char ch) {
    printf("%c%c%c%c\n", ch, ch, ch, ch);
}

int main(int argc, const char** argv) {

    char string[256];

    string[0] = 'M';
    string[1] = 'i';
    string[2] = 'c';
    string[3] = 'h';
    string[4] = 'a';
    string[5] = 'e';
    string[6] = 'l';
    string[7] = '\0';

    printf("%s\n", string);

    char ch = (char)'A';
    outChar(ch);

    exit(0);
}
