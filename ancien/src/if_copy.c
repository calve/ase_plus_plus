#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

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
  unsigned char buffer[BUFFER_SIZE];
  struct file_desc_t file_descriptor;

  if(init_hardware("hardware.ini") == 0) {
    fprintf(stderr, "Error in hardware initialization\n");
    exit(EXIT_FAILURE);
  }

  /* Interrupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;

  if (argc < 2 || sscanf (argv[1], "%i", &inode)!=1){
    fprintf(stderr, "error - argument inode not an integer\n");
    exit(2);
  }
  inode = atoi(argv[1]);

  /* Allows all IT */
  _mask(1);
  load_mbr();
  load_super(current_volume);
  open_ifile(&file_descriptor, inode);
  read(STDIN_FILENO, buffer, BUFFER_SIZE);
  for (i = 0; i < BUFFER_SIZE; i++){
    write_ifile(&file_descriptor, buffer[i]);
  }
  close_ifile(&file_descriptor);
  return(0);
}
