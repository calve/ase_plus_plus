#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "drive.h"
#include "../include/hardware.h"
#include "hda.h"

static void
empty_it()
{
    return;
}

int main(void){
  unsigned int i;
  unsigned char *writing_buffer = malloc(255);
  unsigned char *reading_buffer = malloc(255);
  unsigned char *big_buffer = malloc(2048);

  if(init_hardware("hardware.ini") == 0) {
    fprintf(stderr, "Error in hardware initialization\n");
    exit(EXIT_FAILURE);
  }

  /* Interreupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;

  /* Allows all IT */
  _mask(1);

  printf("Lets do some write and read \n");
  writing_buffer[0] = 'P';
  printf("writing \n");
  write_sector(0, 0, writing_buffer);
  printf("reading \n");
  read_sector(0,0, reading_buffer);
  assert(writing_buffer[0] == reading_buffer[0]);
  printf("buffer is %s (%d)\n", reading_buffer, (int) *reading_buffer);
  printf("will write hello \n");

  writing_buffer[0] = 'h';
  writing_buffer[1] = 'e';
  writing_buffer[2] = 'l';
  writing_buffer[3] = 'l';
  writing_buffer[4] = 'o';

  printf("writing \n");

  write_sector(1, 0, writing_buffer);
  printf("reading \n");
  read_sector(1,0, reading_buffer);
  printf("have read !\n");
  printf("buffer is %s (%d)\n", reading_buffer, (int) *reading_buffer);

  strcpy(big_buffer,"void read_nsector(int cylinder, int sector, unsigned char *buffer, int size){   int i, sector_size;   assert(size);    _out(HDA_CMDREG, CMD_DSKINFO);   sector_size = ((_in(HDA_REGS4)*0xFF) + _in(HDA_REGS5));    seek_sector(cylinder, sector);   _out(HDA_REGS0, (size & 0xFF00));   _out(HDA_REGS1, (size & 0xFF));   _out(HDA_CMDREG, CMD_READ);   _sleep(HDA_IRQ);   for (i=0; i < sector_size; i++){       buffer[i] = MASTERBUFFER[i];   } }  /* Read a sector and put it in buf */ void read_sector(int cylinder, int sector, unsigned char *buffer){   assert(buffer);   read_nsector(cylinder, sector, buffer, 1); }");
  write_sector(2,0, big_buffer);
  read_sector(2,0, big_buffer);
  assert(*writing_buffer == *reading_buffer);
  return 0;
}
