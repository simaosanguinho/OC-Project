#include "L2W2Cache.h"

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
  memset(TempBlock, 0, BLOCK_SIZE); // set all values to 0

  /* init cache */
  if (cache.L1cache.init == 0) {
    for (int i = 0; i < L1_N_LINES; i++) {
      cache.L1cache.lines[i].Valid = 0; // set all valid bits to 0
    }
    cache.L1cache.init = 1; // set init to 1
  }

  CacheL1Line *Lines = cache.L1cache.lines; // get lines from L1

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
  CacheL1Line *Line = &(Lines[Index]);

  if (!Line->Valid || Line->Tag != Tag) {        // if block not present - MISS
    accessL2(address, TempBlock, MODE_READ);     // get new block from DRAM
    if ((Line->Valid) && (Line->Dirty)) {        // if line has dirty block
      MemAddress = ((Line-> Tag) << (indexBits + offsetBits)) + Index;
      accessL2(MemAddress, Line->Data, MODE_WRITE); // then write back old block
    }
    memcpy(&(Line->Data), TempBlock,
           BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;    // set valid bit to 1
    Line->Tag = Tag;    // set tag to the new tag
    Line->Dirty = 0;    // set dirty bit to 0, because we just read the block
  }                     // if miss, then replaced with the correct block

  if (mode == MODE_READ) { // read data from cache line
    memcpy(data, &(Line->Data[Offset]), WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(Line->Data[Offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1; // set dirty bit to 1, because we wrote to the block
  }
}

void accessL2(uint32_t address, unsigned char *data, uint8_t mode) {
  uint32_t Tag, Index, MemAddress, Offset, indexBits, offsetBits;
  unsigned char TempBlock[BLOCK_SIZE];
  memset(TempBlock, 0, BLOCK_SIZE); // set all values to 0

  /* init cache */
  if (cache.L2cache.init == 0) {
    for (int i = 0; i < L2_N_LINES; i++) { // since we have 2 blocks per line
      cache.L2cache.lines[i].Valid[0] = 0;
      cache.L2cache.lines[i].Valid[1] = 0;
      /* The line that will be replace is the one that has the oldest time (= 1)
       */
      cache.L2cache.lines[i].Time[0] = 1;
      cache.L2cache.lines[i].Time[1] = 0;
    }
    cache.L2cache.init = 1;
  }

  CacheL2Line *Lines = cache.L2cache.lines; // get lines from L2
  offsetBits = logBase2(BLOCK_SIZE);        // how many bits for offset
  Offset = address << (32 - offsetBits);    // shift address to the left
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
  CacheL2Line *Line = &(Lines[Index]); // get the line from L2

  int found = 0;         // flag to check if we found the block
  int L2_line_block = 0; // which block in the line we are looking at
  for (int i = 0; i < L2_ASSOC;
       i++) { // iterate over the blocks in the line (associativity)
    if (Line->Valid[i] && Line->Tag[i] == Tag) { // if we found the block
      found = 1;                                 // set flag to 1
      L2_line_block = i;                         // save the block we found
      break;
    }
  }

  if (!found) {                                   // if block not present - Miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from DRAM
    for (int i = 0; i < L2_ASSOC; i++) { // iterate over the blocks in the line
      if (Line->Time[i] == 1) { // if we found the block to replace, aka
                                // the oldest one
        L2_line_block = i;      // save the block we found
        break;
      }
    }

    if ((Line->Valid[L2_line_block]) &&
        (Line->Dirty[L2_line_block])) { // line has dirty block
      MemAddress = ((Line-> Tag[L2_line_block]) << (indexBits + offsetBits)) + Index;
      accessDRAM(MemAddress, &(Line->Data[L2_line_block * BLOCK_SIZE]),
                 MODE_WRITE); // then write back old block to DRAM
    }

    memcpy(&(Line->Data[L2_line_block * BLOCK_SIZE]), TempBlock,
           BLOCK_SIZE);             // copy new block to cache line
    Line->Valid[L2_line_block] = 1; // set valid bit to 1
    Line->Tag[L2_line_block] = Tag; // set tag to the new tag
    Line->Dirty[L2_line_block] = 0; // set dirty bit to 0, because we just read
                                    // the block

  } // if miss, then replaced with the correct block

  if (mode == MODE_READ) { // read data from cache line
    memcpy(data, &(Line->Data[L2_line_block * BLOCK_SIZE]), BLOCK_SIZE);
    // alter last recently used
    if (L2_line_block == 0) { // if we read from the first block
      Line->Time[0] = 0;      // set that first block to the most recently used
      Line->Time[1] = 1;      // and the second block to the least recently used
    } else {                  // else, do the opposite
      Line->Time[0] = 1;
      Line->Time[1] = 0;
    }
    time += L2_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(Line->Data[L2_line_block * BLOCK_SIZE]), data, BLOCK_SIZE);
    // alter last recently used
    if (L2_line_block == 0) { // if we wrote to the first block
      Line->Time[0] = 0;      // set that first block to the most recently used
      Line->Time[1] = 1;      // and the second block to the least recently used
    } else {                  // else, do the opposite
      Line->Time[0] = 1;
      Line->Time[1] = 0;
    }
    time += L2_WRITE_TIME;
    Line->Dirty[L2_line_block] = 1; // set dirty bit to 1, because we wrote to
                                    // the block
  }
}

/*********************** Read and Write *************************/

void read(uint32_t address, unsigned char *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, unsigned char *data) {
  accessL1(address, data, MODE_WRITE);
}
