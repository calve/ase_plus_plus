#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons.h"
#include "file.h"
#include "dir.h"
#include "mount.h"
#include "inode.h"              /* Needed for file_type_e */

/* Maintain a compact list of all builtins commands with their usage
 */
void do_help(){
  printf("List of built-in commands :\n");
  printf("  cat path\n");
  printf("  cd absolute_path\n");
  printf("  compute integer [path]\n");
  printf("  cp source target\n");
  printf("  ed path (the EDitor)\n");
  printf("  exit       -- to exit the shell\n");
  printf("  help\n");
  printf("  ls [path]\n");
  printf("  mkdir path\n");
  printf("  mount volume\n");
  printf("  rm path\n");
  printf("  rmdir path\n");
}


/* Above are listed all builtins commands
 * Keep them ordered !
 */


void do_cat(char* arguments){
  file_desc_t fd;
  int status;
  int c;
  char target[MAXPROMPT];
  canonical_path(target, arguments);

  status = open_file(&fd, target);
  if (status == RETURN_FAILURE){
    printf("Cannot open %s\n", target);
    return;
  }

  while((c=readc_file(&fd)) != READ_EOF)
    putchar(c);

  close_file(&fd);
  printf("\n");
}


void do_cd(char* arguments){
  int status;
  char target[MAXPROMPT];
  canonical_path(target, arguments);

  /* Always append a final '/' */
  if (target[strlen(target)-1] != '/'){
    strcat(target, "/");
  }

  /* Check if directory exists */
  status = inumber_of_path(target);
  if (status == 0){
    printf("Could not find %s\n", target);
  }
  else {
    strcpy(cwd, target);
  }
}

/* A function that do heavy computing used to test contexts */
void do_compute(char* arguments){
  int max, status, inumber, argn;
  char path[MAXPROMPT], target[MAXPROMPT];
  file_desc_t fd;
  argn = sscanf(arguments, "%i %s", &max, path);
  if (argn > 1){
    printf("Will slowly write number until %i to %s\n", max, path);
    canonical_path(target, path);
    inumber = create_file(target, FILE_FILE);
    status = open_ifile(&fd, inumber);
    if (status != RETURN_SUCCESS){
      printf("erreur ouverture fichier %d\n", inumber);
      return;
    }
  }
  for (int i = 0; i<=max; i++){
    second_sleep(i);
    if (argn > 1)
      {
        for (int j = 0; j<i; j++)
          {
            writec_ifile(&fd, '.');
          }
        writec_ifile(&fd, '\n');
        printf("wrote %i dots on %s\n", i, target);
      }
    else
      {
        printf("got %i dots on nothing!\n", i);
      }
  }
  if (argn > 1){
    printf("finished writing on %s\n", target);
    close_file(&fd);
  }
}

void do_cp(char* arguments){
  file_desc_t sfd, dfd;
  unsigned int inumber;
  int status;
  int c;
  char source[MAXPROMPT];
  char dest[MAXPROMPT];

  char canonical_source[MAXPROMPT]="";
  char canonical_dest[MAXPROMPT]="";

  status = sscanf(arguments, "%s %s", source, dest);
  if (status != 2 && status == EOF){
    printf("Error while parsing command line : ``cp %s``\n", arguments);
    return;
  }
  
  canonical_path(canonical_source, source);
  canonical_path(canonical_dest, dest);

  verbose("cp %s %s\n", canonical_source, canonical_dest);

  inumber = create_file(canonical_dest, FILE_FILE);
  if (inumber == RETURN_FAILURE){
    printf("erreur creation fichier\n");
    printf("%u\n", inumber);
    return;
  }

  status = open_ifile(&dfd, inumber);
  if (status != RETURN_SUCCESS){
    printf("erreur ouverture fichier %d\n", inumber);
    return;
  }

  status = open_file(&sfd, canonical_source);
  if (status != RETURN_SUCCESS){
    printf("cannot open %s\n", source);
    return;
  }

  while((c=readc_file(&sfd)) != READ_EOF)
    writec_file(&dfd, c);
  close_file(&dfd);
  close_file(&sfd);
}


/* ed is the standard editor */
void do_ed(char* arguments){
  file_desc_t fd;
  unsigned int inumber;
  int status;
  int c;
  char target[MAXPROMPT];
  canonical_path(target, arguments);

  printf("please wait ...");
  fflush(stdout);
  inumber = create_file(target, FILE_FILE);
  if (inumber == RETURN_FAILURE ){
    printf("erreur creation fichier");
    printf("%u\n", inumber);
    return;
  }
  status = open_ifile(&fd, inumber);
  if (status != RETURN_SUCCESS){
    printf("erreur ouverture fichier %d", inumber);
    return;
  }
  printf(" file opened, you can now enter your text\n");
  while((c=getchar()) != EOF)
    {
      writec_ifile(&fd, c);
    }
  close_file(&fd);
  printf("\n");
}

void do_exit(){
  exit(EXIT_SUCCESS);
}

/* Print the current working directory only for the moment */
void do_ls(char* arguments){
  file_desc_t current;
  struct entry_s entry;
  int counter = 0;
  char target[MAXPROMPT];

  canonical_path(target, arguments);
  open_file(&current, target);
  while (read_ifile (&current, &entry, sizeof(struct entry_s)) != READ_EOF){
    printf("%s ",entry.ent_basename);
    counter++;
  }
  if (counter > 0) {
      printf("\n");
  }
  close_file(&current);
}

void do_mkdir(char* arguments){
  int status;
  char target[MAXPROMPT];

  canonical_path(target, arguments);
  status = create_file(target, FILE_DIRECTORY);
  if (status == RETURN_FAILURE){
    printf("Failed to create %s\n", target);
  }
}

void do_mount(char* arguments){
  int volume;
  sscanf(arguments, "%i", &volume);
  mount_volume(volume);
}

void do_rm(char* arguments){
  int status;
  char target[MAXPROMPT];
  canonical_path(target, arguments);

  status = delete_file(target);
  if (status == RETURN_FAILURE){
    printf("Error removing %s\n", target);
  }
}
