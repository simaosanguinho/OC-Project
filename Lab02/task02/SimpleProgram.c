#include "L1Cache.h"

int main() {

  uint32_t value2, clock;

  initCache();
  initL2Cache();
 
  value2 = 0;

  resetTime();
  write(1, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  read(64, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  read(1, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  read(64*512, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  write(64*512, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  write((64*512)+8192, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  write(1, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  read((64*512)+8192, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  read(1, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();
  read((64*512)+8192, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  return 0;
}
