#include "../include/hardware.h"
#include "hda.h"

void seek_sector(int cylinder, int sector);
void read_sector(int cylinder, int sector, unsigned char *buffer);
void read_nsector(int cylinder, int sector, unsigned char *buffer, int size);
void write_sector(int cylinder, int sector, unsigned char *buffer);
void write_nsector(int cylinder, int sector, unsigned char *buffer, int size);
void format_sector(int cylinder, int sector, int value);
int n_sectors();
