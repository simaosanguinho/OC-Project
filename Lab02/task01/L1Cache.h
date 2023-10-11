#ifndef L1CACHE_H
#define L1CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime();

uint32_t getTime();

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, unsigned char *, uint8_t);

/*********************** Cache *************************/

void initCache();

void accessL1(uint32_t, unsigned char *, uint8_t);

typedef struct CacheLine {
  uint8_t Valid;
  uint8_t Dirty;
  uint32_t Tag;
  unsigned char Data[BLOCK_SIZE];
} CacheLine;

typedef struct Cache {
  uint8_t init;
  CacheLine lines[L1_N_LINES];
} Cache;

/*********************** Interfaces *************************/

void read(uint32_t, unsigned char *);

void write(uint32_t, unsigned char *);

#endif
