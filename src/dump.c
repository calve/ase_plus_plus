#include <stdlib.h>
#include <stdio.h>

#include "drive.h"

void print_sector(int cylinder, int sector){
  unsigned char buffer[255] = "";
  int i;

  read_nsector(cylinder, sector, buffer, 1);
  for (i=0; i<255; i++){
    printf("%02x ",(unsigned int) buffer[i]);
    if (i%8 == 7)
      printf("\n");
  }
}

static void
empty_it()
{
    return;
}

int main(int argc, char** argv){
  unsigned int i;
  int cylinder, sector;

  if (argc < 3){
    printf("usage : %s %s %s\n", argv[0], "cylinder", "sector");
    exit(1);
  }

  /* init hardware */
  if(init_hardware("hardware.ini") == 0) {
    fprintf(stderr, "Error in hardware initialization\n");
    exit(EXIT_FAILURE);
  }

  /* Interreupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;

  /* Allows all IT */
  _mask(1);
  cylinder = atoi(argv[1]);
  sector = atoi(argv[2]);
  print_sector(cylinder, sector);
  fflush(stdout);

  /* and exit! */
  exit(EXIT_SUCCESS);
}
