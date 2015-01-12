#ifndef __INODE_H__
#define __INODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "drive.h"
#include "block.h"
#include "volume.h"

#define NB_INDIRECT 1
#define BLOC_SIZE 255
#define IFILE_FEOF -1
#define BUFFER_SIZE 1024


enum type_e {
  DIR,
  FILES
};

struct inode_s {
  enum type_e type;
  int size;
  int direct[NB_DIRECT];
  int indirect;   /*  If file is */
  int indirect2;  /* toobig      */
};

struct file_desc_t {
  unsigned int inumber;
  unsigned int size;
  unsigned int position;
  unsigned int dirty;
  unsigned char buffer[BLOC_SIZE];
};

int open_ifile(struct file_desc_t* file_descriptor, unsigned int number);
int read_ifile(struct file_desc_t *file_descriptor);
int write_ifile(struct file_desc_t *file_descriptor, char c);
unsigned int create_inode(enum type_e type);
unsigned int f_bloc_of_position(int position);
unsigned int v_bloc_of_f_bloc(int inumber, int f_bloc);
void close_ifile(struct file_desc_t* file_descriptor);
void delete_inode(unsigned int inumber);
void flush_ifile(struct file_desc_t* file_descriptor);
void read_inode(unsigned int inumber);
void seek2_ifile(struct file_desc_t *file_descriptor, int e_offset);

#endif
