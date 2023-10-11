#include "L1Cache.h"

uint8_t L1Cache[L1_SIZE];
uint8_t L2Cache[L2_SIZE];
uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache L1cache;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {

  if (address >= DRAM_SIZE - WORD_SIZE + 1)
    exit(-1);

  if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;
  }

  if (mode == MODE_WRITE) {
    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;
  }
}

/*********************** L1 cache *************************/

void initCache() { L1cache.init = 0; }

int log_base2(int x) {
    if (x == 0) {
        return 0;
    }

    int result = 1.0;
    int increment = 1.0;
    while (x > 2.0) {
        x /= 2.0;
        result += increment;
    }
    return result;
}


void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

  uint32_t Tag, Index, MemAddress, Offset;
  // uint32_t Offset;
  uint8_t TempBlock[BLOCK_SIZE];
  int indexBits, offsetBits;

  /* init cache */
  if (L1cache.init == 0) {
    for (int i = 0; i < L1_N_LINES; i++) {
      L1cache.lines[i].Valid = 0;
    }
    L1cache.init = 1;
  }

  CacheLine *Lines = L1cache.lines;
  indexBits = log_base2(L1_N_LINES); // 8 bits
  printf("Index bits: %d\n", indexBits);
  offsetBits = 6;

  // save offset for later
  Offset = address << (32 - offsetBits);
  Offset = Offset >> (32 - offsetBits);
  printf("\tOFFSET: %d\n", Offset);

  Index = address << (32 - indexBits - offsetBits);
  Index = Index >> (32 - indexBits);

  Tag = address >> (indexBits + offsetBits);
  printf("\tTAG: %d\n", Tag);


  MemAddress = address >> offsetBits; // again this....!
  MemAddress = MemAddress << offsetBits; // address of the block in memory

  /* access Cache*/
  CacheLine *Line = &(Lines[Index]);
  printf("\tIndex: %d\n", Index);

  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from DRAM

    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      accessDRAM(MemAddress, &(L1Cache[Index]), MODE_WRITE); // then write back old block
    }

    memcpy(&(L1Cache[Index]), TempBlock,
           BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block

  if (mode == MODE_READ) {    // read data from cache line
    memcpy(data, &(L1Cache[Index + Offset]), WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(L1Cache[Index + Offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
  }
}

/*********************** Read and Write *************************/

void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
