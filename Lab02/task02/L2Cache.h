#ifndef L2CACHE_H
#define L2CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime();

uint32_t getTime();

int log_base2(int x);

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, unsigned char *, uint32_t);

/*********************** Cache *************************/

void initCache();
void accessL1(uint32_t, unsigned char *, uint32_t);

void accessL2(uint32_t, unsigned char *, uint32_t);

typedef struct CacheLine {
  unsigned char Valid;
  unsigned char Dirty;
  uint32_t Tag;
  unsigned char Data[BLOCK_SIZE];
} CacheLine;


typedef struct L2_Cache {
  uint32_t init;
  CacheLine lines[L2_N_LINES];
} L2_Cache;

typedef struct L1_Cache {
  uint32_t init;
  CacheLine lines[L1_N_LINES];
} L1_Cache;

typedef struct Cache {
  L1_Cache L1cache;
  L2_Cache L2cache;
} Cache;


/*********************** Interfaces *************************/

void read(uint32_t, unsigned char *);

void write(uint32_t, unsigned char *);

#endif
