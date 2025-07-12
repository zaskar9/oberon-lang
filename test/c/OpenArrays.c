#include <stdio.h>

void init(int size, int array[10][10]) {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      array[i][j] = i + j;
    }
  }
}

void print(int size, int array[size][size][size]) {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      printf(" %d", array[i][j][size-1]);
    }
    printf("\n");
  }
}

int main(int argc, char** argv) {
  int matrix[10][10] = { 0 };
  init(10, matrix);
  //print(10, matrix);
}