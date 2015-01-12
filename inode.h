/* ------------------------------
   $Id: inode.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   Inode manipulation.
   Philippe Marquet, october 2002
   
*/

#ifndef _INODE_H_
#define _INODE_H_

#include "vol.h" 	/* for BLOC_SIZE */

enum file_type_e {ordinary, directory, special};

struct inode_s {
    enum file_type_e ind_type;
    unsigned int ind_size; /* in char */
/* Basic inode; a sole inode per BLOC */
#ifndef TEST_D_INDIRECT
#   define N_DIRECT_BLOCS ((BLOC_SIZE/sizeof(int))-4)
#else
#   define N_DIRECT_BLOCS 2
    unsigned int dummy[((BLOC_SIZE/sizeof(int))-6)];     
#endif
    unsigned int ind_direct[N_DIRECT_BLOCS];
    unsigned int ind_indirect;
    unsigned int ind_d_indirect;
};

/* a bloc full of zeros */
#define BLOC_NULL 0

/* number of bloc number in a bloc */
#define NBLOC_PER_BLOC ((BLOC_SIZE/sizeof(int)))

void read_inode (unsigned int inumber, struct inode_s *inode);
void write_inode (unsigned int inumber, struct inode_s *inode);

/* return the bloc index on the volume of a given bloc index in a
   file.  
   Return BLOC_NULL for a bloc full of zeros */ 
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc); 

/* allocate and return a bloc on the volume (in order to write in the
   file).
   This may imply indirect and d_indirect bloc creation.
   Return BLOC_NULL if no allocation was possible. */
unsigned int allocate_vbloc_of_fbloc(unsigned int inumber, unsigned int bloc);

#endif
