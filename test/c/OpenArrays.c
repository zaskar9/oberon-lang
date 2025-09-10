#include <stdio.h>
#include <string.h>

int m[100];
int l[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

void print(int array[], int len) {
  for (int i = 0; i < len; ++i) {
      printf("%3d", array[i]);
  }
  printf("\n");
}

void Test2(int matrix[], int array[], int len) {
  memcpy(&matrix[10], array, len);
}

void Test3(int matrix[], int array[], int len) {
  Test2(matrix, array, len);
}

int main(int argc, char** argv) {
  int matrix[10 * 10] = { 0 };
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      matrix[10 * i + j] = 10 * i + j;
    }
  }
  for (int i = 0; i < 10; ++i) {
    print(&matrix[i * 10], 10);
  }
  printf("\n");
  // int l[10]; = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
  Test2(m, l, sizeof l);
  // memcpy(&matrix[10], array, sizeof array);
  for (int i = 0; i < 10; ++i) {
    print(&m[i * 10], 10);
  }
}