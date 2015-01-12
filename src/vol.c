#include "vol.h"

void load_super(unsigned int volume){
  current_volume = volume;
  read_nblock(volume, 0, (unsigned char*) &superblock, sizeof(superblock));
  assert(superblock.canary == (int) CANARY);
}

void save_super(){
  assert(superblock.canary == (int) CANARY);
  write_nblock(current_volume, 0, (unsigned char*) &superblock, sizeof(superblock));
}

void init_super(char* name,  unsigned int serial) {
  int i;

  load_mbr();

  superblock.canary = CANARY;
  superblock.root_block = 0;
  superblock.serial = serial;
  superblock.first_free = 1;
  strncpy(superblock.name, name, 32);
  current_volume = mbr.n_volume-1;

  for (i = 1; i < (mbr.volumes[current_volume].nbloc); i++){
    struct free_block_s block;
    block.canary = CANARY;
    block.next = (i < mbr.volumes[current_volume].nbloc - 1 ? i+1 : BLOC_NULL);
    write_nblock(current_volume, i, (unsigned char*) &block, sizeof(block));
  }
  save_super();
}

/* Returns an unallocated block
 */
unsigned int new_block() {
  struct free_block_s free_block;
  int first_free;

  if (superblock.first_free == BLOC_NULL){
      printf("disk full\n");
      return 0;
  }
  first_free = superblock.first_free;
  read_nblock(current_volume, first_free, (unsigned char*)&free_block, sizeof(free_block));
  if (free_block.next != BLOC_NULL) {
    superblock.first_free = free_block.next;
  } else {
    superblock.first_free = BLOC_NULL;
  }

  save_super();
  return first_free;
}

/* Free a block and insert it in the block list
 */
void free_block(int block){
  struct free_block_s free_block;

  printf("freeing_block\n");

  free_block.next = superblock.first_free;
  write_nblock(current_volume,block,(unsigned char*)& free_block, sizeof(free_block));
  superblock.first_free = block;

  save_super();
}
