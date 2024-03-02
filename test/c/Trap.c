//
// Created by Michael Grossniklaus on 2/29/24.
//

#include <signal.h>
#include <stdio.h>

int main(void) {
    raise(SIGABRT);
    printf("Hello World.");
}
