#include "inode.h"


unsigned int f_bloc_of_position(int position){
  return position / BLOC_SIZE;
}

unsigned int v_bloc_of_fbloc(int inumber, int f_bloc){
  struct inode_s inode;
  unsigned int t[NB_INDIRECT];
  read_block(current_volume, inumber, (unsigned char *) &inode);
  if (f_bloc < NB_DIRECT){
    return inode.direct[f_bloc];
  }
  f_bloc = f_bloc - NB_DIRECT;
  if (f_bloc < NB_INDIRECT){
    read_block(current_volume, inode.indirect, (unsigned char*) &t);
    return t[f_bloc];
  }
  f_bloc = f_bloc - NB_INDIRECT;
  if (f_bloc < (NB_INDIRECT * NB_INDIRECT)){
    read_block(current_volume, inode.indirect2, (unsigned char*) t);
    read_block(current_volume, t[f_bloc/NB_INDIRECT], (unsigned char*) t);
    return t[f_bloc % NB_INDIRECT];
  }
  perror("no such v_bloc");
  exit( -1);
}

void seek2_ifile(struct file_desc_t *file_descriptor, int e_offset){
  assert(file_descriptor->position + e_offset);
  if (f_bloc_of_position(file_descriptor->position) != f_bloc_of_position(file_descriptor->position + e_offset)){
    flush_ifile(file_descriptor);
    read_block(current_volume,
               v_bloc_of_fbloc(file_descriptor->inumber,
                                f_bloc_of_position(file_descriptor->position + e_offset)),
               file_descriptor->buffer);
    file_descriptor->dirty = 0;
  }
  file_descriptor->position += e_offset;
}
