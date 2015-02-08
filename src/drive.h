#ifndef DRIVE
#define DRIVE

#define HW_CONFIG "hardware.ini"

void seek_sector(unsigned int cylinder, unsigned int sector);
void read_sector(unsigned int cylinder, unsigned int sector, unsigned char *buffer);
void read_nsector(unsigned int cylinder, unsigned int sector, unsigned char *buffer, int size);
void write_sector(unsigned int cylinder, unsigned int sector, unsigned char *buffer);
void write_nsector(unsigned int cylinder, unsigned int sector, unsigned char *buffer, int size);
void format_sector(unsigned int cylinder, unsigned int sector, unsigned int nsector, unsigned int value);
void dump(unsigned char *buffer, unsigned int buffer_size, int ascii_dump, int octal_dump); 

#endif
