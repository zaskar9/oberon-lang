#include <stdlib.h>

int halt(int code) {
    if (code != 0) {
        exit(code);
    }
    return code;
}

int main(void) {
    halt(1);
}