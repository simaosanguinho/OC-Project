#ifndef 2WL2CACHE_H
#define 2WL2CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime();

uint32_t getTime();

int log_base2(int x);

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t);

/*********************** Cache *************************/

void initCache();
void accessL1(uint32_t, uint8_t *, uint32_t);

void initL2Cache();
void accessL2(uint32_t, uint8_t *, uint32_t);

typedef struct CacheLine {
  uint8_t Valid;
  uint8_t Dirty;
  uint32_t Tag;
} CacheLine;

typedef struct CacheL2Line {
  uint8_t Valid[2];
  uint8_t Dirty[2];
  uint32_t Tag[2];
  uint8_t Time[2];
} CacheL2Line;

typedef struct Cache {
  uint32_t init;
  CacheLine lines[L1_N_LINES];
} Cache;

typedef struct L2_Cache {
  uint32_t init;
  CacheL2Line lines[L2_N_LINES];
} L2_Cache;


/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *);

void write(uint32_t, uint8_t *);

#endif
