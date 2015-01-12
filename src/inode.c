#include "inode.h"

unsigned int create_inode(enum type_e type){
  struct inode_s inode;
  unsigned int inumber;
  int i;

  inode.type = type;
  inode.size = 0;
  for(i = 0; i < NB_DIRECT; i++){
    inode.direct[i] = BLOC_NULL;
  }
  inode.indirect = BLOC_NULL;
  inode.indirect2 = BLOC_NULL;
  inumber = new_block();
  if (!inumber){
    return BLOC_NULL;
  }
  write_nblock(current_volume, inumber, (unsigned char*) &inode, sizeof(inode));
  return inumber;
}

void free_array(unsigned int* t, int size){
  int i;
  for (i=0; i < size; i++){
    if (t[i]){
      free_block(t[i]);
    }
  }
}

void delete_inode(unsigned int inumber){
  /* On peut éviter la fragmentation en parcourant nos 'for' à l'envers */
  struct inode_s inode;
  unsigned int t[NB_INDIRECT];
  unsigned int t2[NB_INDIRECT];
  int i;

  read_nblock(current_volume, inumber, (unsigned char*) &inode, sizeof(inode));
  free_array((unsigned int*) inode.direct, NB_DIRECT);
  if (inode.indirect != BLOC_NULL){
    read_block(current_volume, inode.indirect, (unsigned char*) &t);
    free_array(t, NB_INDIRECT);
    free_block(inode.indirect);
  }
  if (inode.indirect2 != BLOC_NULL){
    read_block(current_volume, inode.indirect2, (unsigned char*) &t2);
    for (i=0; i < NB_DIRECT; i++){
      if (t2[i] != BLOC_NULL){
        read_block(current_volume, t2[i], (unsigned char*) &t);
        free_array(t, NB_INDIRECT);
        free_block(t2[i]);
      }
    }
    free_block(inode.indirect2);
  }
  free_block(inumber);
}


int open_ifile(struct file_desc_t* file_descriptor, unsigned int number){
  struct inode_s inode;
  read_nblock(current_volume, number, (unsigned char*) &inode, sizeof(inode));
  file_descriptor->inumber = number;
  file_descriptor->size = inode.size;
  file_descriptor->position = 0;
  if (inode.direct[0] == BLOC_NULL){
    inode.direct[0] = new_block();
    write_nblock(current_volume, number, (unsigned char*) &inode, sizeof(inode));
  }
  read_block(current_volume, inode.direct[0], file_descriptor->buffer);
  file_descriptor->dirty = 0;
  return 0;
}


/* Flush file_descriptor if dirty
 */
void flush_ifile(struct file_desc_t* file_descriptor){
  if (file_descriptor->dirty){
    unsigned int f_bloc;
    unsigned int v_bloc;
    struct inode_s inode;
    read_nblock(current_volume, file_descriptor->inumber, (unsigned char*) &inode, sizeof(inode));
    inode.size = file_descriptor->size;
    file_descriptor->dirty = 1;
    f_bloc = f_bloc_of_position(file_descriptor->position);
    v_bloc = v_bloc_of_f_bloc(file_descriptor->inumber, f_bloc);
    write_block(current_volume, v_bloc, file_descriptor->buffer);
    write_nblock(current_volume, file_descriptor->inumber, (unsigned char*) &inode, sizeof(inode));
  }
}

/* Close file_descriptor
 */
void close_ifile(struct file_desc_t* file_descriptor){
  flush_ifile(file_descriptor);
  printf("closed file\n");
}

unsigned int f_bloc_of_position(int position){
  return position / BLOC_SIZE;
}

unsigned int v_bloc_of_f_bloc(int inumber, int f_bloc){
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
               v_bloc_of_f_bloc(file_descriptor->inumber,
                                f_bloc_of_position(file_descriptor->position + e_offset)),
               file_descriptor->buffer);
    file_descriptor->dirty = 0;
  }
  file_descriptor->position += e_offset;
}

int read_ifile(struct file_desc_t *file_descriptor){
  unsigned char res;
  if (file_descriptor->position >= file_descriptor->size){
    return IFILE_FEOF;
  }
  res = file_descriptor->buffer[file_descriptor->position % BLOC_SIZE];

  seek2_ifile(file_descriptor, 1);
  return res;
}

int write_ifile(struct file_desc_t *file_descriptor, char c){
  file_descriptor->buffer[file_descriptor->position] = c;
  file_descriptor->dirty = 1;
  if (file_descriptor->position +1 > file_descriptor->size){
    file_descriptor->size++;
  }
  seek2_ifile(file_descriptor, 1);
  return 0;
}
