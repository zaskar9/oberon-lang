#include <stdlib.h>
#include <stdio.h>

void write(const char* msg)
{
   printf("%s", msg);
}

int main(int argc, const char* argv[])
{
   write("Hello, World!\n");
   exit(0);
}

// gcc -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti -fverbose-asm -O -S helloworld.c