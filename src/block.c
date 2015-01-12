#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "drive.h"
#include "block.h"

void load_mbr()
{
  read_nsector(0,0,(unsigned char*)&mbr, sizeof(mbr));
  if (mbr.canary != MBR_CANARY)
    {
      mbr.canary = MBR_CANARY;
      mbr.n_volume = 0;
    }
}

int sector_of_block(int volume, int block)
{
  assert(mbr.canary == MBR_CANARY);
  assert(volume < MAX_VOLUMES);
  return (mbr.volumes[volume].first_sector + block) % n_sectors();
}

int cylinder_of_block(int volume, int block)
{
  assert(mbr.canary == MBR_CANARY);
  assert(volume < MAX_VOLUMES);
  return mbr.volumes[volume].first_cylinder + ((mbr.volumes[volume].first_sector + block) / n_sectors());
}

void read_block(int volume, int block, unsigned char* buffer)
{
  read_sector(cylinder_of_block(volume, block), sector_of_block(volume, block), buffer);
}

void read_nblock(int volume, int block, unsigned char* buffer, int size)
{
  read_nsector(cylinder_of_block(volume, block), sector_of_block(volume, block), buffer, size);
}

void write_block(int volume, int block, unsigned char* buffer)
{
  write_sector(cylinder_of_block(volume, block), sector_of_block(volume, block), buffer);
}

void write_nblock(int volume, int block, unsigned char* buffer, int size)
{
  write_nsector(cylinder_of_block(volume, block), sector_of_block(volume, block), buffer, size);
}

void format_block(int volume, int block, int value)
{
  format_sector(cylinder_of_block(volume, block), sector_of_block(volume, block), value);
}

/** Create a new volume starting at cylinder,sector with size in bytes
 */
void mkvol(int cylinder, int sector, int size, int type){
  struct vol_desc_s volume;

  if (mbr.n_volume >= MAX_VOLUMES){
    perror("Maximum volume reached. Exiting");
    exit(1);
  }

  if (cylinder == 0 && sector == 0){
    perror("Cannot write volume to mbr. Exiting");
    exit(4);
  }
  /** Verify volume do not overlap
   */
  if (mbr.n_volume != 0){
    int min_cylinder, min_sector, max_sectors;
    struct vol_desc_s previous_volume;
    max_sectors = SECTOR_SIZE;
    previous_volume = mbr.volumes[mbr.n_volume-1];
    min_cylinder = previous_volume.first_cylinder + (previous_volume.nbloc / max_sectors);
    min_sector = previous_volume.first_sector + (previous_volume.nbloc % max_sectors);
    if (cylinder <= min_cylinder && sector < min_sector){
      fprintf(stderr,"Cannot create a volume here !\n");
      fprintf(stderr,  "min cylinder : %d \nmin sector: %d\n", min_cylinder, min_sector);
      exit(1);
    }
  }

  volume.first_cylinder = cylinder;
  volume.first_sector = sector;
  volume.nbloc = size;

  switch (type){
    case VOLTYPE_BASE :
      volume.vol_type = base;
      break;
    case VOLTYPE_ANNEX :
      volume.vol_type = annex;
      break;
    case VOLTYPE_OTHER :
      volume.vol_type = other;
      break;
  default:
    perror("unknow type");
    exit(3);
  }

  mbr.volumes[mbr.n_volume] = volume;
  mbr.n_volume++;

  write_nsector(0, 0, (unsigned char*) &mbr, sizeof(mbr));
}

