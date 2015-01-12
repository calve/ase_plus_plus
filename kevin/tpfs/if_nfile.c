/* ------------------------------
   $Id: if_nfile.c,v 1.1 2009/11/16 05:38:07 marquet Exp $
   ------------------------------------------------------------

   Create a new file, read from stdin, return the inumber 
   Philippe Marquet, Nov 2009
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "file.h"
#include "mount.h"

static void
nfile()
{
    file_desc_t fd;
    unsigned int inumber;
    int status;
    int c;
    char nom[16];
    printf("%s\n", "Saisissez un nom de fichier :");
    scanf("%s", nom);
    inumber = create_file(nom, FILE_FILE);
    ffatal(inumber, "erreur creation fichier");
    printf("%u\n", inumber);

    status = open_ifile(&fd, inumber);
    ffatal(!status, "erreur ouverture fichier %d", inumber);
    
    while((c=getchar()) != EOF)
        writec_ifile(&fd, c);

    close_file(&fd);
}

static void
usage(const char *prgm)
{
    fprintf(stderr, "[%s] usage:\n\t"
            "%s\n", prgm, prgm);
    exit(EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
    if (argc != 1)
        usage(argv[0]);

    mount();
    nfile();
    umount();
    
    exit(EXIT_SUCCESS);         
}
