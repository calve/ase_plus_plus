#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons.h"
#include "file.h"
#include "dir.h"
#include "mount.h"
#include "inode.h"              /* Needed for file_type_e */
#include "builtins.h"

/* The list of available command name, the function they call, and an inline help
 */
COMMAND commands[] = {
  { "cat", do_cat, "Print file FILE" },
  { "cd", do_cd, "Change to directory DIR" },
  { "compute", do_compute, "Compute stuffs DIR" },
  { "cp", do_cp, "cp source dest DIR" },
  { "ed", do_ed, "edit FILE" },
  { "help", do_help, "Display this text" },
  { "?", do_help, "Synonym for `help'" },
  { "ls", do_ls, "Synonym for `list'" },
  { "mkdir", do_mkdir, "Make a directory" },
  { "mount", do_mount, "Mount a volume" },
  { "rm", do_rm, "Remove a volume" },
  { "exit", do_exit, "Quit" },
  { (char *)0, (void *)0, (char *)0 }
};

/* Print out help for ARG, or for all of the commands if ARG is
   not present. */
int do_help (char* arg) {
  register int i;
  int printed = 0;

  for (i = 0; commands[i].name; i++)
    {
      if (!*arg || (strcmp (arg, commands[i].name) == 0))
        {
          printf ("%s\t\t%s.\n", commands[i].name, commands[i].doc);
          printed++;
        }
    }

  if (!printed)
    {
      printf ("No commands match `%s'.  Possibilties are:\n", arg);

      for (i = 0; commands[i].name; i++)
        {
          /* Print in six columns. */
          if (printed == 6)
            {
              printed = 0;
              printf ("\n");
            }

          printf ("%s\t", commands[i].name);
          printed++;
        }

      if (printed)
        printf ("\n");
    }
  return 0;
}



/* Above are listed all builtins commands
 * Keep them ordered !
 */


int do_cat(char* arguments){
  file_desc_t fd;
  int status;
  int c;
  char target[MAXPROMPT];
  canonical_path(target, arguments);

  status = open_file(&fd, target);
  if (status == RETURN_FAILURE){
    printf("Cannot open %s\n", target);
    return 1;
  }

  while((c=readc_file(&fd)) != READ_EOF)
    putchar(c);

  close_file(&fd);
  printf("\n");
  return 0;
}


int do_cd(char* arguments){
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
  return 0;
}

/* A function that do heavy computing used to test contexts */
int do_compute(char* arguments){
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
      return 1;
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
  return 0;
}

int do_cp(char* arguments){
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
    return 1;
  }
  
  canonical_path(canonical_source, source);
  canonical_path(canonical_dest, dest);

  verbose("cp %s %s\n", canonical_source, canonical_dest);

  inumber = create_file(canonical_dest, FILE_FILE);
  if (inumber == RETURN_FAILURE){
    printf("erreur creation fichier\n");
    printf("%u\n", inumber);
    return 1;
  }

  status = open_ifile(&dfd, inumber);
  if (status != RETURN_SUCCESS){
    printf("erreur ouverture fichier %d\n", inumber);
    return 1;
  }

  status = open_file(&sfd, canonical_source);
  if (status != RETURN_SUCCESS){
    printf("cannot open %s\n", source);
    return 1;
  }

  while((c=readc_file(&sfd)) != READ_EOF)
    writec_file(&dfd, c);
  close_file(&dfd);
  close_file(&sfd);
  return 0;
}


/* ed is the standard editor */
int do_ed(char* arguments){
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
    return 1;
  }
  status = open_ifile(&fd, inumber);
  if (status != RETURN_SUCCESS){
    printf("erreur ouverture fichier %d", inumber);
    return 1;
  }
  printf(" file opened, you can now enter your text\n");
  while((c=getchar()) != EOF)
    {
      writec_ifile(&fd, c);
    }
  close_file(&fd);
  printf("\n");
  return 0;
}

int do_exit(char* arguments){
  exit(EXIT_SUCCESS);
  return 0;
}

/* Print the current working directory only for the moment */
int do_ls(char* arguments){
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
  return 0;
}

int do_mkdir(char* arguments){
  int status;
  char target[MAXPROMPT];

  canonical_path(target, arguments);
  status = create_file(target, FILE_DIRECTORY);
  if (status == RETURN_FAILURE){
    printf("Failed to create %s\n", target);
  }
  return 0;
}

int do_mount(char* arguments){
  int volume;
  sscanf(arguments, "%i", &volume);
  mount_volume(volume);
  return 0;
}

int do_rm(char* arguments){
  int status;
  char target[MAXPROMPT];
  canonical_path(target, arguments);

  status = delete_file(target);
  if (status == RETURN_FAILURE){
    printf("Error removing %s\n", target);
  }
  return 0;
}
