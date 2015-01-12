#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "drive.h"

static void
empty_it()
{
    return;
}

int main(int argc, char** argv){
  int i,j, m_cylinders, m_sectors;
  int reverse = 0;
  int format_bytes = 0xdeadbeaf;

  /* Parse `-r` option */
  if (argc >= 2 && argv[1][0] == '-' && argv[1][0] == 'r' ){
    reverse = 1;
  }

  if(init_hardware("hardware.ini") == 0) {
    fprintf(stderr, "Error in hardware initialization\n");
    exit(EXIT_FAILURE);
  }

  /* Interrupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;

  /* Allows all IT */
  _mask(1);

  printf("Begin format …");

  /* Get disk size */
  _out(HDA_CMDREG, CMD_DSKINFO);
  m_cylinders = (_in(HDA_REGS0)*0xFF) + _in(HDA_REGS1);
  m_sectors = (_in(HDA_REGS2)*0xFF) + _in(HDA_REGS3);
  printf("got %d cylinders and %d sectors\n", m_cylinders, m_sectors);
  fflush(stdout);
  if (reverse == 0){
    for (i=0; i < m_cylinders; i++){
      for (j=0; j < m_sectors; j++){
        format_sector(i, j, format_bytes);
      }
    }
  }
  else {
    for (i=m_cylinders; i > 0; i--){
      for (j=m_sectors; j > 0; j--){
        format_sector(i, j, format_bytes);
      }
    }
  }
  printf("… ended\n");
  exit(0);
}
