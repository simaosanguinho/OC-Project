#include "L2Cache.h"

unsigned char DRAM[DRAM_SIZE];
uint32_t time;
Cache cache;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, unsigned char *data, uint8_t mode) {
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

/*********************** L1 & L2 cache *************************/
void initCache() {
  cache.L1cache.init = 0;
  cache.L2cache.init = 0;
}

uint8_t logBase2(int x) { // used to get the number of bits
  if (x == 0) {
    return 0;
  }

  uint8_t result = 1;
  uint8_t increment = 1;
  while (x > 2) {
    x /= 2;
    result += increment;
  }
  return result;
}

void accessL1(uint32_t address, unsigned char *data, uint8_t mode) {
  uint32_t Tag, Index, MemAddress, Offset, indexBits, offsetBits;
  unsigned char TempBlock[BLOCK_SIZE];
  memset(TempBlock, 0, BLOCK_SIZE);

  /* init cache */
  if (cache.L1cache.init == 0) {
    for (int i = 0; i < L1_N_LINES; i++) {
      cache.L1cache.lines[i].Valid = 0;
    }
    cache.L1cache.init = 1;
  }

  CacheLine *Lines = cache.L1cache.lines; // get lines from L1

  offsetBits = logBase2(BLOCK_SIZE);     // how many bits for offset
  Offset = address << (32 - offsetBits); // shift address to the left
  Offset =
      Offset >>
      (32 - offsetBits); // shift address to the right, so we get the offset

  indexBits = logBase2(L1_N_LINES);                 // how many bits for index
  Index = address << (32 - indexBits - offsetBits); // shift address to the left
  Index = Index >>
          (32 - indexBits); // shift address to the right, so we get the index

  Tag = address >>
        (indexBits + offsetBits); // get tag, by shifting address to the right

  MemAddress =
      address >>
      offsetBits; // shift address to the right, so we can remove the offset
  MemAddress = MemAddress << offsetBits; // address of the block in memory

  /* access Cache*/
  CacheLine *Line = &(Lines[Index]);

  if (!Line->Valid || Line->Tag != Tag) {        // if block not present - miss
    accessL2(address, TempBlock, MODE_READ);     // get new block from DRAM
    if ((Line->Valid) && (Line->Dirty)) {        // line has dirty block
      uint32_t address_on_L1 = ((Line-> Tag) << (indexBits + offsetBits)) + Index;
      accessL2(address_on_L1, Line->Data, MODE_WRITE_BLOCK); // then write back old block
    }
    memcpy(&(Line->Data), TempBlock,
           BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;  // set valid bit to 1
    Line->Tag = Tag;  // set tag to the new tag
    Line->Dirty = 0;  // set dirty bit to 0, because we just read the block
  } // if miss, then replaced with the correct block

  if (mode == MODE_READ) { // read data from cache line
    memcpy(data, &(Line->Data[Offset]), WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(Line->Data[Offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;  // set dirty bit to 1, because we wrote to the block
  }
}

void accessL2(uint32_t address, unsigned char *data, uint8_t mode) {
  uint32_t Tag, Index, MemAddress, Offset, indexBits, offsetBits;
  unsigned char TempBlock[BLOCK_SIZE];

  /* init cache */
  if (cache.L2cache.init == 0) {
    for (int i = 0; i < L2_N_LINES; i++) {
      cache.L2cache.lines[i].Valid = 0;
    }
    cache.L2cache.init = 1;
  }

  CacheLine *Lines = cache.L2cache.lines;

  offsetBits = logBase2(BLOCK_SIZE);     // how many bits for offset
  Offset = address << (32 - offsetBits); // shift address to the left
  Offset =
      Offset >>
      (32 - offsetBits); // shift address to the right, so we get the offset

  indexBits = logBase2(L2_N_LINES);                 // how many bits for index
  Index = address << (32 - indexBits - offsetBits); // shift address to the left
  Index = Index >>
          (32 - indexBits); // shift address to the right, so we get the index

  Tag = address >>
        (indexBits + offsetBits); // get tag, by shifting address to the right

  MemAddress =
      address >>
      offsetBits; // shift address to the right, so we can remove the offset
  MemAddress = MemAddress << offsetBits; // address of the block in memory

  /* access Cache*/
  CacheLine *Line = &(Lines[Index]);

  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from DRAM

    if ((Line->Valid) && (Line->Dirty)) { // if line has dirty block
      MemAddress = ((Line-> Tag) << (indexBits + offsetBits)) + Index;
      accessDRAM(MemAddress, Line->Data,
                 MODE_WRITE); // then write back old block
    }

    memcpy(&(Line->Data), TempBlock,
           BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;    // set valid bit to 1
    Line->Tag = Tag;    // set tag to the new tag
    Line->Dirty = 0;    // set dirty bit to 0, because we just read the block
  }                     // if miss, then replaced with the correct block

  if (mode == MODE_READ) { // read data from cache line
    memcpy(data, &(Line->Data[Offset]), WORD_SIZE);
    time += L2_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(Line->Data[Offset]), data, WORD_SIZE);
    time += L2_WRITE_TIME;
    Line->Dirty = 1; // set dirty bit to 1, because we wrote to the block
  }

  if (mode == MODE_WRITE_BLOCK) {
    memcpy(&(Line->Data[0]), data, BLOCK_SIZE);
    time += L2_WRITE_TIME;
    Line->Dirty = 1; // set dirty bit to 1, because we wrote to the block
  }
}

/*********************** Read and Write *************************/

void read(uint32_t address, unsigned char *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, unsigned char *data) {
  accessL1(address, data, MODE_WRITE);
}
