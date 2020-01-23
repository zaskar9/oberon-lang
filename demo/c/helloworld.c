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

int compute()
{
   int a = 0, b = 0;
   int* x = &a;
   int* y = &b;
   printf("%d\n", x);
   printf("%d\n", y);
   return *x + *y;
}

int compare(int a, int b, int c, int d, int e, int f)
{
   return a < b && c < d;
   /*if (a < b)
   {
      return c + d;
   }
   else
   {
      return e + f;
   }*/
}

// gcc -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti -fverbose-asm -O -S helloworld.c
