//
// Created by Michael Grossniklaus on 3/2/23.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef char String[255];

typedef struct {
    long len;
    String str;
} TString;

typedef struct {
    TString first;
    TString last;
} PersonDesc;

String string, s3;
char *s4, *s5;

const TString hw = { .len = 13, .str = "Hello World!" };
const TString hn = { .len = 13, .str = "Hoi Niklaus!" };
const TString hermione = { .len = 9, .str = "Hermione" };
const TString granger = { .len = 8, .str = "Granger" };

TString s1;

PersonDesc *people[2];

void printStr(const TString *value) {
    int i = 0;
    while (i < value->len && value->str[i] != '\0') {
        printf("%c", value->str[i++]);
    }
}

void createPerson(const TString *first, const TString *last, PersonDesc *p) {
    p->first = *first;
    p->last = *last;
}

void printPerson(PersonDesc *p) {
    printStr(&p->first); printf(" "); printStr(&p->last);
}

void printPeople(PersonDesc *people[]) {
    //for (int i = 0; i < 2; ++i) {
        printPerson(people[0]); // putchar('\n');
    //}
}

void swap(int *i, int *j) {
    int t = *i;
    *i = *j;
    *j = t;
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

//    s4 = (char*) hw.str;
//    s5 = (char*) hn.str;
//    printf("s4: %s\n", s4);
//    printf("s5: %s\n", s5);
//    printStr(hw.str, 100);
//    printf("\n");

//    s1 = hw;
//    printf("s1: %s\n", s1.str);

    PersonDesc pd;
    createPerson(&hermione, &granger, &pd);
    people[0] = &pd;

    PersonDesc pd2;
    pd2.first = hermione;
    pd2.last = granger;
    people[1] = &pd2;

    printPeople(people); putchar('\n');

    int a = 10;
    int b = 20;
    swap(&a, &b);
    printf("%d, %d\n", a, b);

    TString str;
    str.len = 25;
    memcpy(&str.str, &hermione.str, hermione.len);
    printf("%s\n", str.str);

    int c = a > b ? a : b;
    exit(0);
}
