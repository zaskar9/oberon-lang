//
// Created by Michael Grossniklaus on 2025-01-08.
//

#include <stdio.h>

int case_statement(char ch) {
    switch (ch) {
        case 0: return printf("(null)");
        case 1: return printf("(start of heading)");
        case 2: return printf("(start of text)");
        case 3: return printf("(end of text)");
        case 4: return printf("(end of transmission)");
        case 5: return printf("(enquiry)");
        case 6:
        case 7:
        case 8:
        case 9:
        case 10: return printf("labels 6 thru 10");
        default: return printf("unknown character");
    }
}

int main() {
    char ch = 0;
    while (ch != '\n') {
        ch = fgetc(stdin);
        case_statement(ch);
        printf("\n");
    }
    return 0;
}
