/* This file contains a shell aimed to interact with our kernel
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#include "../include/hardware.h"
#include "hardware_ini.h"
#include "file.h"
#include "dir.h"
#include "mount.h"
#include "inode.h"              /* Needed for file_type_e */
#include "ctx.h"

#define MAXPROMPT 256

/* This macro should be used like printf
 * it will output only if we are in verbose mode
 */
#define verbose(format, ...) do {                 \
    if (is_verbose)                               \
      fprintf(stdout, format, ##__VA_ARGS__);     \
  } while(0)

char cwd[MAXPROMPT] = "/";
const char* shellsymbol = ">";
int is_verbose = 0;


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


static void
timer_it() {
  _out(TIMER_ALARM, 0xFFFFFFFE);
  yield();
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
  if (index){
    return index+1;
  }
  else
    return "";
}

/* Returns true if the command is in background
 */
int is_background(char* command_line){
  char* index = strchr(command_line, '&');
  if (index)
    return 1;
  else
    return 0;
}

/* Construct the canonical path of ``path`` and store it in ``canonical``
 * returns a canonical path that starts at ``/`` and should not contains ``.`` (dot) or ``..`` (double dot)
 */
void canonical_path(char* canonical, char* path){
  char *token;
  char target[MAXPROMPT] = "";
  strcpy(canonical, target);
  if (*path != '/'){
    strcpy(target, cwd);
    strcat(target, path);
  } else {
    strcpy(target, path);
  }
  verbose("canonical path : %s\n", target);

  /* Parse target to find double-dot sequences */
  token = strtok(target, "/");
  while(token != NULL){
    if (strncmp(token, "..", 2) == 0){ /* Go one level up */
      char tmp[MAXPROMPT] = "";
      int last_index;
      strcpy(tmp, canonical);
      last_index = strrchr(tmp, '/') - tmp;
      canonical[0] = '\0';
      if (strlen(tmp) == 1)
        {
          canonical = strncat(canonical, "/", 1);
        }
      else
        {
          strncat(canonical, tmp, last_index);
        }
    }
    else if (strncmp(token, ".", 1) == 0){ /* Do nothing */ }
    else { /* Concat this token to the target*/
      strcat(canonical, "/");
      strcat(canonical, token);
    }
    verbose("canonical path : %s\n", canonical);
    token = strtok(NULL, "/");
  }
  if (strlen(canonical) == 0){ /* Ensure canonical path is not empty */
    strncat(canonical, "/", 1);
  }
}

/* Try to open the file. If you can open the file, the name is already taken. Else you can create a file with this name
 */
int exists(char* arguments){
  file_desc_t fd;
  int status;
  char target[MAXPROMPT];
  canonical_path(target, arguments);

  status = open_file(&fd, target);
  if (status == RETURN_FAILURE){
    return 0;
  }
  close_file(&fd);
  return 1;
}


/* Maintain a compact list of all builtins commands with their usage
 */
void do_help(){
  printf("List of built-in commands :\n");
  printf("  cat path\n");
  printf("  cd absolute_path\n");
  printf("  compute integer\n");
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
  int res = 0;
  int max;
  max = atoi(arguments);
  for (int i = 0; i<max; i++){
    for (int j = i; j<max; j++){
      res += j;
    }
  }
  printf("finished computing, result %i\n", res);
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

  if (exists(canonical_dest)){
    printf("Impossible to copy the file with this name. Please try an other one.\n");
    return;
  }

  verbose("cp %s %s\n", canonical_source, canonical_dest);

  inumber = create_file(canonical_dest, FILE_FILE);
  if (inumber == RETURN_FAILURE){
    printf("erreur creation fichier");
    printf("%u\n", inumber);
    return;
  }

  status = open_ifile(&dfd, inumber);
  if (status != RETURN_SUCCESS){
    printf("erreur ouverture fichier %d", inumber);
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

  if (exists(target)){
    printf("Impossible to create a file with this name. Please try an other one.\n");
    return;
  }

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


/* Evaluate a command runned inside the shell
 */
int eval(char *cmd){
  char *arguments = get_arguments(cmd);

  if(!is_command(cmd, "cat")){
    do_cat(arguments);
  }
  else if(!is_command(cmd, "cd")){
    do_cd(arguments);
  }
  else if(!is_command(cmd, "compute")){
    do_compute(arguments);
  }
  else if(!is_command(cmd, "cp")){
    do_cp(arguments);
  }
  else if(!is_command(cmd, "ed")){
    do_ed(arguments);
  }
  else if(!is_command(cmd, "help")){
    do_help();
  }
  else if(!is_command(cmd, "ls")){
    do_ls(arguments);
  }
  else if(!is_command(cmd, "mkdir")){
    do_mkdir(arguments);
  }
  else if(!is_command(cmd, "mount")){
    do_mount(arguments);
  }
  else if(!is_command(cmd, "rm")){
    do_rm(arguments);
  }
  else if(!is_command(cmd, "exit")){
    do_exit();
  }
  else{
    printf("Unknow command\n");
    return 1;
  }
  return 0;
}


void shell_loop(void* arguments) {
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
        /* Copy the command line so it is not freed to soon */
        char cmdline_cpy[MAXPROMPT] = "";
        strcpy(cmdline_cpy, cmdline);

        add_history(cmdline_cpy);
        if (is_background(cmdline_cpy)){
          verbose("Will run ``%s`` in a new context\n", cmdline_cpy + 1);
          create_ctx(16000, (void*) eval, cmdline_cpy + 1);
        }
        else {
          eval(cmdline_cpy);
        }
        fflush(stdout);
        fflush(stdin);
      }

    free(cmdline); /* Has been allocated by readline */
  }
}

int main(int argc, char** argv){
  /* Initialize history */
  using_history();

  if (argc > 1 && strcmp(argv[1], "-v") == 0){
    is_verbose = 1;
    printf("Set verbose flag\n");
  } else {
    printf("Verbose flag not set. Use -v if you want to\n");
  }

  printf("Welcome in shell. Build date %s %s\n", __DATE__, __TIME__);
  printf("Type ``help`` to find out all the available commands in this shell\n");
  mount_volume(0);
  printf("Volume 0 has been automatically mounted. Use ``mount`` to mount another\n");
  IRQVECTOR[TIMER_IRQ] = timer_it;
  _out(TIMER_PARAM,128+64); /* reset + alarm on + 8 tick / alarm */
  _out(TIMER_ALARM,0xFFFFFFFD);  /* alarm at next tick (at 0xFFFFFFFF) */
  printf("Binded timer interruptions\n");
  printf("Add ``&`` in front of a command to run it in a new context (in background)\n");
  printf("\n");

  create_ctx(16000, shell_loop, NULL);
  start_sched();
  umount();
  fflush(stdout);
  exit(EXIT_SUCCESS);
}
