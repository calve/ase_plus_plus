#ifndef __VOLUME_H__
#define __VOLUME_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "drive.h"
#include "block.h"

#define NB_DIRECT (((SECTOR_SIZE-8)/4))
#define CANARY (0xFA115AFE)
#define BLOC_NULL 0
#define BLOC_SIZE 255 /* This value may not be accurate */

struct super_b {
  int canary;
  int serial;
  char name[32];
  int root_block;  /* Where is root ? */
  int first_free;
};

struct super_b superblock;
int current_volume;

struct free_block_s {
  int canary;
  int next;
};


unsigned int new_block();
void free_block(int bloc);
void init_super(char* name,  unsigned int serial);
void load_super(unsigned int volume);
void save_super();

#endif
