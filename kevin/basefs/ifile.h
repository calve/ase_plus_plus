/* ------------------------------
   $Id: ifile.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   File manipulation. 
   Basic version; manipulate files via their inode number. 
   Philippe Marquet, october 2002
   
*/

#ifndef _IFILE_H_
#define _IFILE_H_

#include "cfile.h"
#include "inode.h"

/*------------------------------
  Initialization and finalization
  ------------------------------------------------------------*/

/* a binary must mount a volume/file system before any other operations.
   A sole file system mount is allowed.
   The file system must be umount.
*/

int mount(unsigned int vol);
int umount();

/*------------------------------
  File creation and deletion
  ------------------------------------------------------------*/

/* return the inumber of the newly created file, 0 if error. */
unsigned int create_ifile(enum file_type_e type);
/* free all blocs associated to the inode */
int delete_ifile(unsigned int inumber);

/*------------------------------
  File opening
  ------------------------------------------------------------*/

int open_ifile(file_desc_t *fd, unsigned int inumber);

/*------------------------------
  Functions copied from sfile. 
  ------------------------------------------------------------*/

#include "sfile.h"

/* no inline keyword in ANSI, sorry */
#define close_ifile(fd) close_sfile((fd))
#define flush_ifile(fd) flush_sfile((fd))
#define seek_ifile(fd,offset) seek_sfile((fd), (offset))
#define seek2_ifile(fd,offset) seek2_sfile((fd), (offset))
#define readc_ifile(fd) readc_sfile((fd))
#define writec_ifile(fd,c) writec_sfile((fd), (c))
#define read_ifile(fd,buf,nbyte) read_sfile((fd),(buf),(nbyte))
#define write_ifile(fd,buf,nbyte) write_sfile((fd),(buf),(nbyte))

/*------------------------------
  Internal fucntions
  ------------------------------------------------------------*/

/* the inode is already loaded, shortcut the begining of open_ifile() */
int iopen_ifile(file_desc_t *fd, unsigned int inumber, struct inode_s *inode);


#endif
