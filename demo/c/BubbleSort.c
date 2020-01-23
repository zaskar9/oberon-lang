// Implementation of the Bubble Sort algorithm
#include <stdlib.h>
#include <stdio.h>

// Length of the array to be sorted.
const int DIM = 2 * 10;

// Array to be sorted.
int a[DIM];
struct point2D
{
   int x;
   int y;
};
struct point2D points[DIM];

// Initializes the array.
void init()
{
   int i = 0;
   while (i < DIM)
   {
      a[i] = DIM - i;
      i++;
   }
}

// Swaps the two values passed as var-parameters.
void swap(int* a, int* b)
{
   int t = *a;
   *a = *b;
   *b = t;
}

void bubble()
{
   int i = 0;
   while (i < DIM) 
   {
      int j = DIM - 1;
      while (j > i) 
      {
         if (a[j-1] > a[j]) 
         {
            swap(&a[j-1], &a[j]);
         }
         j--;
      }
      i++;
   }
}

void printArray(const int a[])
{
   int i = 0;
   printf("[");
   while (i < DIM)
   {
      printf("%d", a[i]);
      i++;
      if (i < DIM) printf(", ");
   }
   printf("]\n");
}

void printPoint(const struct point2D p)
{
   printf("(%d, %d)", p.x, p.y);
}

int main(int argc, const char* argv[])
{
   init();
   bubble();
   printArray(a);
   struct point2D p;
   p.x = 10;
   p.y = 20;
   printPoint(p);
   exit(0);
}

// gcc -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti -fverbose-asm -fomit-frame-pointer -O0 -S BubbleSort.c 