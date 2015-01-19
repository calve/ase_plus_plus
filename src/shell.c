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
  return strchr(command_line, ' ')+1;;
}

/* Above are listed all builtins commands
 * Keep them ordered !
 */

void do_help(){
  printf("List of built-in commands :\n");
  printf("  help\n");
  printf("  ls\n");
  printf("  mount volume\n");
  printf("  mkdir path\n");
}

/* Print the current working directory only for the moment */
void do_ls(){
  file_desc_t current;
  struct entry_s entry;
  open_file(&current, cwd);
  while (read_ifile (&current, &entry, sizeof(struct entry_s)) != READ_EOF){
    printf("%s ",entry.ent_basename);
  }
  printf("\n");
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
  if(!is_command(cmd, "help")){
    do_help();
    return 0;
  }
  if(!is_command(cmd, "ls")){
    do_ls();
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

  printf("\nType help to find out all the available commands in this shell\n\n");

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
      fflush(stdout);
      exit(EXIT_SUCCESS);
    }


    /* Evaluate the command line */
    if (cmdline[0] != '\0') /* evaluate the command line only it is not empty */
      {
	add_history(cmdline);
	eval(cmdline);
	fflush(stdout);
      }

    free(cmdline); /* As been allocated by readline */
  }
  umount();
  exit(EXIT_SUCCESS); /* control never reaches here */
}
