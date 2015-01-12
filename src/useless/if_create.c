#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "block.h"
#include "volume.h"
#include "inode.h"


static void
empty_it()
{
  return;
}

int main(int argc, char** argv){
  int i, inode;

  if(init_hardware("hardware.ini") == 0) {
    fprintf(stderr, "Error in hardware initialization\n");
    exit(EXIT_FAILURE);
  }

  /* Interrupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;

  /* Allows all IT */
  _mask(1);
  load_mbr();
  load_super(current_volume);
  inode = create_inode(FILES);
  printf("%d\n",inode);
  return(0);
}
