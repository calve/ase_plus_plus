/* This file contains a shell aimed to interact with our kernel
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#include "file.h"
#include "dir.h"
#include "mount.h"
#include "inode.h"              /* Needed for file_type_e */

#define MAXPROMPT 256

char cwd[MAX_DIR_SIZE] = "/";
const char* shellsymbol = ">";

int construct_prompt(char* string, int string_size)
{
  char res[MAXPROMPT]="";
  if (string_size < MAXPROMPT)
    {
      return 1;
    }
  strcat(res,cwd);
  strcat(res,shellsymbol);
  strcpy(string,res);
  return 0;
}

/* Test if this command line should invoque command
 * examples :
 * is_command("mkdir plop plop", "mkdir") -> true
 * is_command("mkdir", "mkdir") -> true
 * is_command("mk dir", "mkdir") -> false
 */
int is_command(char* command_line, char* command){
  return strncmp(command_line, command, strlen(command));
}


/* Returns a pointer to the first argument of a command line
 */
char* get_arguments(char* command_line){
  char* index = strchr(command_line, ' ');
  if (index)
    return index+1;
  else
    return "";
}

/* Construct the canonical path of ``path`` and store it in ``target``
 * returns a canonical path that starts at ``/`` and should not contains ``.`` (dot) or ``..`` (double dot)
 */
void canonical_path(char* target, char* path){
  if (*path != '/'){
    strcpy(target, cwd);
    strcat(target, path);
  } else {
    strcpy(target, path);
  }
}

/* Maintain a compact list of all builtins commands with their usage
 */

void do_help(){
  printf("List of built-in commands :\n");
  printf("  cat\n");
  printf("  cd\n");
  printf("  ed (the EDitor)\n");
  printf("  help\n");
  printf("  ls\n");
  printf("  mount volume\n");
  printf("  mkdir path\n");
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
  ffatal(!status, "erreur ouverture fichier %s", target);

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

/* ed is the standart editor */
void do_ed(char* arguments){
  file_desc_t fd;
  unsigned int inumber;
  int status;
  int c;
  char target[MAXPROMPT];
  canonical_path(target, arguments);

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
  printf("your move :\n");
  while((c=getchar()) != EOF)
    {
      writec_ifile(&fd, c);
    }
  close_file(&fd);
  printf("\n");
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
  status = create_file(arguments, FILE_DIRECTORY);
  if (status == RETURN_FAILURE){
    printf("Failed to create %s\n", arguments);
  }
}

void do_mount(char* arguments){
  int volume;
  sscanf(arguments, "%i", &volume);
  mount_volume(volume);
}


/* Evaluate a command runned inside the shell
 */
int eval(char *cmd){
  char *arguments = get_arguments(cmd);
  if(!is_command(cmd, "cat")){
    do_cat(arguments);
    return 0;
  }
  if(!is_command(cmd, "cd")){
    do_cd(arguments);
    return 0;
  }
  if(!is_command(cmd, "ed")){
    do_ed(arguments);
    return 0;
  }
  if(!is_command(cmd, "help")){
    do_help();
    return 0;
  }
  if(!is_command(cmd, "ls")){
    do_ls(arguments);
    return 0;
  }
  if(!is_command(cmd, "mkdir")){
    do_mkdir(arguments);
    return 0;
  }
  if(!is_command(cmd, "mount")){
    do_mount(arguments);
    return 0;
  }
  printf("Unknow command\n");
  return 1;
}


int main(int argc, char** argv){
  /* Initialize history */
  using_history();

  printf("Type ``help`` to find out all the available commands in this shell\n\n");
  mount_volume(0);
  printf("Volume 0 has been automatically mounted. Use ``mount`` to mount another\n");
  /* Execute the shell's read/eval loop */
  while (1) {
    char prompt[MAXPROMPT];
    char* cmdline;

    /* Reload prompt each time */
    construct_prompt(prompt,MAXPROMPT);

    /* Read command line */
    cmdline = readline(prompt);

    if (!cmdline) {    /* End of file (ctrl-d) */
      printf("\n");    /* Give a clean prompt to user */
      break;
    }


    /* Evaluate the command line */
    if (cmdline[0] != '\0') /* evaluate the command line only it is not empty */
      {
        add_history(cmdline);
        eval(cmdline);
        fflush(stdout);
        fflush(stdin);
      }

    free(cmdline); /* Has been allocated by readline */
  }
  umount();
  fflush(stdout);
  exit(EXIT_SUCCESS);
}
