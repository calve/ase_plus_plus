#ifndef __BLOCK_H__
#define __BLOCK_H__

#define SECTOR_SIZE (255)  /* Hardware dependant */
#define MBR_CANARY 0xdeadbeef
#define MAX_VOLUMES 8

#define VOLTYPE_BASE 0
#define VOLTYPE_ANNEX 1
#define VOLTYPE_OTHER 2

enum vol_type_e {
  base,
  annex,
  other
};

struct vol_desc_s {
  int first_cylinder;
  int first_sector;
  int nbloc;
  enum vol_type_e vol_type;
};

struct mbr_s {
  unsigned int canary;
  int n_volume;
  struct vol_desc_s volumes[MAX_VOLUMES];
};

struct mbr_s mbr;
void load_mbr();
int sector_of_block(int volume, int block);
int cylinder_of_block(int volume, int block);
void read_block(int volume, int block, unsigned char* buffer);
void read_nblock(int volume, int block, unsigned char* buffer, int size);
void write_block(int volume, int block, unsigned char* buffer);
void write_nblock(int volume, int block, unsigned char* buffer, int size);
void format_block(int volume, int block, int value);
void mkvol(int cylinder, int sector, int size, int type);

#endif
