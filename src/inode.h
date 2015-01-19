/* ------------------------------
   $Id: inode.h,v 1.2 2009/11/17 09:44:12 marquet Exp $
   ------------------------------------------------------------

   Inode manipulation interface.
   Philippe Marquet, Nov 2009

*/

#ifndef _INODE_H_
#define _INODE_H_

#include "hardware_ini.h"
#include "tools.h"

#define NB_DIRECTS (HDA_SECTORSIZE/4)-4
#define BLOC_SIZE       HDA_SECTORSIZE
#define DATA_BLOC_SIZE  BLOC_SIZE
#define NB_BLOCS_BY_BLOC  NB_DIRECTS+4
#define HW_CONFIG "hardware.ini"

/* different kind of files */
enum file_type_e {FILE_FILE, FILE_DIRECTORY, FILE_SPECIAL};

/* inode */
struct inode_s{
	enum file_type_e type;
	unsigned int size;
	unsigned int blocs_directs[NB_DIRECTS];
	unsigned int blocs_indirect;
	unsigned int blocs_double_indirect;
};


/* inodes i/o */
void read_inode (unsigned int inumber, struct inode_s *inode);
void write_inode (unsigned int inumber, struct inode_s *inode);

/* inodes creation/deletion.
   The creation return a inumber */
unsigned int create_inode(enum file_type_e type);
int delete_inode(unsigned int inumber);

/* return the bloc index on the volume of a given bloc index in a
   file.  Return BLOC_NULL for a bloc full of zeros.
   If do_allocate, allocate blocs if needed; return BLOC_NULL if no
   allocation was possible.  */
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc,
                            bool_t do_allocate);

unsigned initialize_block(unsigned adress);
void free_blocs(unsigned int *blocs, unsigned int size);

#endif
