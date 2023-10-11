#ifndef L2W2CACHE_H
#define L2W2CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime();

uint32_t getTime();

void printL1();
void printL2();
void printDRAM();

uint8_t logBase2(int x);

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, unsigned char *, uint8_t);

/*********************** Cache *************************/

void initCache();

void accessL1(uint32_t, unsigned char *, uint8_t);

void accessL2(uint32_t, unsigned char *, uint8_t);

typedef struct CacheL1Line {
  uint8_t Valid;
  uint8_t Dirty;
  uint32_t Tag;
  unsigned char Data[BLOCK_SIZE];
} CacheL1Line;

typedef struct CacheL2Line {
  uint8_t Valid[2];
  uint8_t Dirty[2];
  uint32_t Tag[2];
  uint8_t Time[2];
  unsigned char Data[BLOCK_SIZE * 2];
} CacheL2Line;

typedef struct L2W2_Cache {
  uint8_t init;
  CacheL2Line lines[L2_N_LINES];
} L2_Cache;

typedef struct L1_Cache {
  uint8_t init;
  CacheL1Line lines[L1_N_LINES];
} L1_Cache;

typedef struct Cache {
  L1_Cache L1cache;
  L2_Cache L2cache;
} Cache;


/*********************** Interfaces *************************/

void read(uint32_t, unsigned char *);

void write(uint32_t, unsigned char *);

#endif
