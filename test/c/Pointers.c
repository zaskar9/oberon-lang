//
// Created by Michael Grossniklaus on 10/3/22.
//

#include <stdlib.h>
#include <stdio.h>

struct person {
    char* name;
    char* address;
    int zipcode;
};

struct node {
    int value;
    struct node *left, *right;
};

int *ptr1, *ptr2;
struct person *michael;
struct node *root, *child;

void printPerson(struct person *person) {
    char* name = person->name;
    printf("%s\n", name);
}

int main(int argc, const char* argv[]) {

//    ptr1 = NULL;
//    ptr2 = NULL;
//    ptr1 = (int*) malloc(sizeof(int));
//    *ptr1 = 10;
//    ptr2 = ptr1;
//    printf("%d\n", *ptr2);

    michael = (struct person *) malloc(sizeof(struct person));
    michael->name = "Michael Grossniklaus";
    printPerson(michael);

//    root = (struct node *) malloc(sizeof(struct node));
//    root->value = 5;
//    child = (struct node *) malloc(sizeof(struct node));
//    child->value = 10;
//    root->right = child;
//    int test = root->right->value;
//    printf("%d\n", test);

    return EXIT_SUCCESS;

}

// clang -S -emit-llvm Pointers.c