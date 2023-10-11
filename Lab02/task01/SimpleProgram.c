#include "L1Cache.h"

int main() {

  uint32_t value2, clock;

  resetTime();
  initCache();
  //value1 = -1;
  value2 = 0;

  read(1, (uint8_t *)(&value2));

  clock = getTime();
  printf("Time: %d\n", clock);

  resetTime();

  read(63, (uint8_t *)(&value2));

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
  

  return 0;
}
