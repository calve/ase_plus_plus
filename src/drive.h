#ifndef DRIVE
#define DRIVE

#include "hardware_ini.h"
#include "../include/hardware.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HW_CONFIG "hardware.ini"

void seek_sector(unsigned int cylinder, unsigned int sector);
void read_sector(unsigned int cylinder, unsigned int sector, unsigned char *buffer);
void write_sector(unsigned int cylinder, unsigned int sector, const unsigned char *buffer);
void format_sector(unsigned int cylinder, unsigned int sector, unsigned int nsector, unsigned int value);
void dump(unsigned char *buffer, unsigned int buffer_size, int ascii_dump, int octal_dump); 

#endif
