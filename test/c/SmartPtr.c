//
// Created by Michael Grossniklaus on 3/11/22.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DEREF(type, ptr) *((type*) ptr.ref)

struct smart_ptr {
    void *ref;
    int *cnt;
};

struct smart_ptr new(void *raw_ptr) {
    int *cnt = (int*) malloc(sizeof(int));
    *cnt = 0;
    struct smart_ptr ptr = { raw_ptr, cnt };
    return ptr;
}

void delete(struct smart_ptr ptr) {
    free((void*) ptr.ref);
    free(ptr.cnt);
}

void unassign(struct smart_ptr ptr) {
    --*ptr.cnt;
    if (*ptr.cnt == 0) {
        printf("delete %d\n", *ptr.cnt);
        delete(ptr);
    }
}

void assign(struct smart_ptr *lhs, struct smart_ptr rhs) {
    if (lhs->cnt != NULL) {
        (*(lhs->cnt))--;
    }
    (*rhs.cnt)++;
    *lhs = rhs;
}

int main(int argc, const char* argv[]) {

    struct smart_ptr ptr = { NULL, NULL };

    assign(&ptr, new((float*) malloc(sizeof(float))));
    DEREF(float, ptr) = 100.0;

    struct smart_ptr ptr1 = { NULL, NULL };
    struct smart_ptr ptr2 = { NULL, NULL };
    assign(&ptr1, ptr);
    assign(&ptr2, ptr1);

    printf("%d, %d, %d\n", *ptr.cnt, *ptr1.cnt, *ptr2.cnt);
    printf("%f, %f, %f\n", DEREF(float, ptr), DEREF(float, ptr1), DEREF(float, ptr2));

    DEREF(float, ptr) = 200.0;
    printf("%f, %f, %f\n", DEREF(float, ptr), DEREF(float, ptr1), DEREF(float, ptr2));

    struct smart_ptr ptr3 = { NULL, NULL };
    assign(&ptr3, new ((float*) malloc(sizeof(float))));
    DEREF(float, ptr3) = 300.0;

    printf("%d, %d, %d, %d\n", *ptr.cnt, *ptr1.cnt, *ptr2.cnt, *ptr3.cnt);
    printf("%f, %f, %f, %f\n", DEREF(float, ptr), DEREF(float, ptr1), DEREF(float, ptr2), DEREF(float, ptr3));

    assign(&ptr, ptr3);
    printf("%d, %d, %d, %d\n", *ptr.cnt, *ptr1.cnt, *ptr2.cnt, *ptr3.cnt);
    printf("%f, %f, %f, %f\n", DEREF(float, ptr), DEREF(float, ptr1), DEREF(float, ptr2), DEREF(float, ptr3));

    // end of scope: unassign all local pointer
    unassign(ptr);
    printf("%d, %d, %d, %d\n", *ptr.cnt, *ptr1.cnt, *ptr2.cnt, *ptr3.cnt);
    unassign(ptr1);
    printf("%d, %d, %d, %d\n", *ptr.cnt, *ptr1.cnt, *ptr2.cnt, *ptr3.cnt);
    unassign(ptr2);
    printf("%d, %d, %d, %d\n", *ptr.cnt, *ptr1.cnt, *ptr2.cnt, *ptr3.cnt);
    unassign(ptr3);
    printf("%d, %d, %d, %d\n", *ptr.cnt, *ptr1.cnt, *ptr2.cnt, *ptr3.cnt);

}
