/* ------------------------------
   $Id: file.c 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   File manipulation. The main library interface.
   Philippe Marquet, october 2002
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbr.h"
#include "superbloc.h"
#include "ifile.h"
#include "dir.h"
#include "file.h"

/*------------------------------
  File creation and deletion
  ------------------------------------------------------------*/

int
create_file(const char *pathname, enum file_type_e type)
{
    unsigned int idir;
    unsigned int inumber; 
    const char *basename;
    int status;
    /* does the directory exist? */
    idir = dinumber_of_path(pathname, &basename); 
    if (! idir)
      return RETURN_FAILURE;

    /* We should not be able to open this pathname as it should not exists yes */
    inumber = inumber_of_path(pathname);
    if (inumber != 0){
        printf("Error creating %s, file already exists\n", pathname);
        return RETURN_FAILURE;
    }

    /* Okay, this pathname does not exists yet, create it ! */
    inumber = create_ifile(type);
    if (! inumber)
      return RETURN_FAILURE;

    /*  link the file in his directory */
    status = add_entry(idir, inumber, basename);
    if (status != RETURN_SUCCESS)
      return RETURN_FAILURE;
    return inumber;
}

int
delete_file(const char *pathname)
{
    unsigned int idir;
    const char *basename;
    int status;
    
    /* does the directory exist? */
    idir = dinumber_of_path(pathname, &basename); 
    if (! idir)
	return RETURN_FAILURE;

    /* suppress the entry in the directory */
    status = del_entry(idir, basename); 
    
    return status;
}

/*------------------------------
  File management
  ------------------------------------------------------------*/

int
open_file(file_desc_t *fd, const char *pathname)
{
    unsigned int inumber; 
    int status;

    /* convert the pathname into an inumber */  
    inumber = inumber_of_path(pathname);
    if (! inumber)
	return RETURN_FAILURE; 

    /* and open teh file */ 
    status = open_ifile(fd, inumber);

    return status;
}


int list_directory(char subentries[][MAXPATH], int size, const char *pathname){
    file_desc_t current;
    int counter = 0;
    struct entry_s entry;

    open_file(&current, pathname);
    for (int i = 0; i<size; i++){
        strcpy(subentries[counter], "");
    }
    while (counter < size && read_ifile (&current, &entry, sizeof(struct entry_s)) != READ_EOF){
        struct inode_s inode;
        read_inode(entry.ent_inumber, &inode);
        strcpy(subentries[counter], entry.ent_basename);
        if (inode.type == FILE_DIRECTORY){
            strcat(subentries[counter], "/");
        }
        counter++;
    }
    close_file(&current);
    return counter;
}

void
close_file(file_desc_t *fd)
{
    close_ifile(fd);
}

void
flush_file(file_desc_t *fd)
{
    flush_ifile(fd);
}

void
seek_file(file_desc_t *fd, int offset)
{
    seek_ifile(fd, offset);
}

/*------------------------------
  File accesses
  ------------------------------------------------------------*/

int
readc_file(file_desc_t *fd)
{
    return readc_ifile(fd);
}

int
writec_file(file_desc_t *fd, char c)
{
    return writec_ifile(fd, c);
}

int
read_file(file_desc_t *fd, void *buf, unsigned int nbyte)
{
    return read_ifile(fd, buf, nbyte);
}

int
write_file(file_desc_t *fd, const void *buf, unsigned int nbyte)
{
    return write_ifile(fd, buf, nbyte);
}

