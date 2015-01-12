#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "volume.h"

#define TABLE_HEAD      " volume | cylinder | sector |    size | type | name\n"
#define TABLE_SEPARATOR "--------+----------+--------+---------+------+-----\n"
#define TABLE_ROW       "    %03d |      %03d |    %03d |     %03d |   %s |%s \n"


static void
empty_it()
{
    return;
}


int main(){
  int i;
  struct vol_desc_s current_volume;
  char * vol_type_string[3];

  if(init_hardware("hardware.ini") == 0) {
    fprintf(stderr, "Error in hardware initialization\n");
    exit(EXIT_FAILURE);
  }

  /* Interrupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;

  vol_type_string[VOLTYPE_BASE] = "base";
  vol_type_string[VOLTYPE_ANNEX] = "annex";
  vol_type_string[VOLTYPE_OTHER] = "other";

  /* Allows all IT */
  _mask(1);
  load_mbr();
  printf(TABLE_HEAD);
  printf(TABLE_SEPARATOR);
  for (i=0; i<mbr.n_volume; i++){
    current_volume = mbr.volumes[i];
    load_super(i);
    printf(TABLE_ROW, i, current_volume.first_cylinder, current_volume.first_sector, current_volume.nbloc, vol_type_string[current_volume.vol_type], superblock.name);
  }
  return 0;
}
