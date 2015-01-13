#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "drive.h"
#include "block.h"

int load_mbr()
{
  read_nsector(0,0,(unsigned char*)&mbr, sizeof(mbr));
  if (mbr.canary != MBR_CANARY)
    {
      mbr.canary = MBR_CANARY;
      mbr.n_volume = 0;
    }
  return 0;
}
