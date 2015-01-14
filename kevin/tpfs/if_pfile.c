/* ------------------------------
   $Id: if_pfile.c,v 1.1 2009/11/16 05:38:07 marquet Exp $
   ------------------------------------------------------------

   Print a file identified by its inumber
   Philippe Marquet, Nov 2009
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "file.h"
#include "mount.h"

static void
pfile(const char *pathname)
{
    file_desc_t fd;
    int status;
    int c;
    
    status = open_file(&fd, pathname);
    ffatal(!status, "erreur ouverture fichier %s", pathname);

    while((c=readc_file(&fd)) != READ_EOF)
        putchar(c);

    close_file(&fd);
}

static void
usage(const char *prgm)
{
    fprintf(stderr, "[%s] usage:\n\t"
            "%s name\n", prgm, prgm);
    exit(EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
    unsigned inumber;
    if (argc < 2)
        usage(argv[0]);
    printf("%s\n", argv[1]);

    mount();
    pfile(argv[1]);
    umount();
    
    exit(EXIT_SUCCESS);         
}
