#include <stdio.h>

void print(int array[], int len) {
  for (int i = 0; i < len; ++i) {
      printf("%3d", array[i]);
  }
  printf("\n");
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
}