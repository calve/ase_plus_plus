#include <assert.h>
#include <stdio.h>
#include "drive.h"

void seek_sector(int cylinder, int sector){
  int hs = (sector >> 8) & 0xFF;
  int ls = sector & 0xFF;
  int hc = (cylinder >> 8) & 0xFF;
  int lc = cylinder & 0xFF;
  assert(hs >= 0);
  assert(ls >= 0);
  assert(hc >= 0);
  assert(lc >= 0);
  _out(HDA_REGS0, hc);
  _out(HDA_REGS1, lc);
  _out(HDA_REGS2, hs);
  _out(HDA_REGS3, ls);
  _out(HDA_CMDREG, CMD_SEEK);
  _sleep(HDA_IRQ);
  /* Hurry up now */
}

/* Reads size bytes
 *
 */
void read_nsector(int cylinder, int sector, unsigned char *buffer, int size){
  int i;
  assert(size);
  seek_sector(cylinder, sector);
  _out(HDA_REGS0, 0);
  _out(HDA_REGS1, 1);
  _out(HDA_CMDREG, CMD_READ);
  _sleep(HDA_IRQ);
  for (i=0; i < size; i++){
    buffer[i] = MASTERBUFFER[i];
  }
}

/* Read a sector and put it in buf */
void read_sector(int cylinder, int sector, unsigned char *buffer){
  int sector_size;
  assert(buffer);
  _out(HDA_CMDREG, CMD_DSKINFO);
  sector_size = ((_in(HDA_REGS4)*0xFF) + _in(HDA_REGS5));
  read_nsector(cylinder, sector, buffer, sector_size);
}


/* Write size bytes
 */
void write_nsector(int cylinder, int sector, unsigned char *buffer, int size){
  int i;

  assert(size);
  assert(buffer);

  for (i=0; i < size; i++){
    MASTERBUFFER[i] = buffer[i];
  }

  seek_sector(cylinder, sector);


  _out(HDA_REGS0, (size & 0xFF00));
  _out(HDA_REGS1, (size & 0xFF));
  _out(HDA_CMDREG, CMD_WRITE);
  _sleep(HDA_IRQ);
}


void write_sector(int cylinder, int sector, unsigned char *buffer){
  int sector_size;
  assert(buffer);
  _out(HDA_CMDREG, CMD_DSKINFO);
  sector_size = ((_in(HDA_REGS4)*0xFF) + _in(HDA_REGS5));
  write_nsector(cylinder, sector, buffer, sector_size);
}

void format_sector(int cylinder, int sector, int value){
  printf("format cylinder = %d, sector = %d\n", cylinder, sector);
  seek_sector(cylinder, sector);
  _out(HDA_REGS0, 0);
  _out(HDA_REGS1, 1);
  _out(HDA_REGS2, (value >> 24) & 0xff);
  _out(HDA_REGS3, (value >> 16) & 0xff);
  _out(HDA_REGS4, (value >> 8) & 0xff);
  _out(HDA_REGS5, value & 0xff);
  _out(HDA_CMDREG, CMD_FORMAT);
  _sleep(14);
}

/* Returns the number of sectors in a cylinder

 */
int n_sectors()
{
  /* int sector_size; */
  /* _out(HDA_CMDREG, CMD_DSKINFO); */
  /* sector_size = ((_in(HDA_REGS4)*0xFF) + _in(HDA_REGS5)); */
  /* return sector_size; */
  return 16;
}
