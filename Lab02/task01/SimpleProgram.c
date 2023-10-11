#include "L1Cache.h"


int two_raise_x(int x) {
  int result = 1;
  for (int i = 0; i < x; i++) {
    result *= 2;
  }
  return result;
}

int main() {
  uint32_t value2;
  printf("\n");

  for (int i = 0; i < 13; i++) {
    initCache();
    resetTime();
    printf("Number of words: %d\n", two_raise_x(i));
    value2 = 0;
    for (int j = 0; j < two_raise_x(i); j++) {
      value2 = j * 4;
      write(j*4, (uint8_t *)(&value2));
      printf("Write; Address %d; Value %d; Time %d\n", j*4, value2, getTime());
    }
    value2 = 0;
    for (int j = 0; j < two_raise_x(i); j++) {
      value2 = j * 4;
      read(j*4, (uint8_t *)(&value2));
      printf("Read; Address %d; Value %d; Time %d\n", j*4, value2, getTime());
    }
    printf("\n");
  }

  return 0;
}
