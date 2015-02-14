/* This file contains a shell aimed to interact with our kernel
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#include "../include/hardware.h"
#include "hardware_ini.h"
#include "ctx.h"
#include "commons.h"
#include "builtins.h"
#include "mount.h"
#include "file.h"
#include "hw.h"

const char* shellsymbol = ">";
char *command_generator PARAMS((const char *, int));
char *list_generator PARAMS((const char *, int));
char **shell_completion PARAMS((const char *, int, int));

extern struct ctx_s *waiting_ctx;

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


/* Tell the GNU Readline library how to complete.  We want to try to complete
   on command names if this is the first word in the line, or on filenames
   if not. */
void initialize_readline() {
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "ashell";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = shell_completion;
}

/* Attempt to complete on the contents of TEXT.  START and END bound the
   region of rl_line_buffer that contains the word to complete.  TEXT is
   the word to complete.  We can use the entire contents of rl_line_buffer
   in case we want to do some simple parsing.  Return the array of matches,
   or NULL if there aren't any. */
char** shell_completion (const char *text, int start, int end){
  char **matches;

  matches = (char **)NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  if (start == 0)
    matches = rl_completion_matches (text, command_generator);
  else
    matches = rl_completion_matches (text, list_generator);

  return (matches);
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char* command_generator (const char *text, int state)
{
       static int list_index, len;
       char *name;

       /* If this is a new word to complete, initialize now.  This includes
          saving the length of TEXT for efficiency, and initializing the index
          variable to 0. */
       if (!state)
         {
           list_index = 0;
           len = strlen (text);
         }

       /* Return the next name which partially matches from the command list. */
       while ((name = commands[list_index].name))
         {
           list_index++;

           if (strncmp (name, text, len) == 0)
             return (strdup(name));
         }

       /* If no names matched, then return NULL. */
       return ((char *)NULL);
}

/* Generator function for files completion.
 */
char* list_generator (const char *text, int state)
{
    static int list_index, len;
    static int last_slash = 0;
    char *name;
    static char subentries[MAXPATH][MAXPATH];
    static int entry_lenght;
    char destination[MAXPROMPT] = "";
    char target[MAXPROMPT];

    /* If this is a new word to complete, initialize now.  This includes
       saving the length of TEXT for efficiency, and initializing the index
       variable to 0. */
    if (!state)
        {
            int i;
            len = strlen (text);
            for (i = len-1; i>=0; i--){
                if (text[i] == '/'){
                    last_slash = i;
                    break;
                }
            }
            if (last_slash > 0){
                strcat(destination, cwd);
                strncat(destination, text, last_slash);
                canonical_path(target, destination);
             }
            else {
                canonical_path(target, cwd);
            }
            entry_lenght = list_directory(subentries, 1024, target);
            list_index = 0;
        }
    /* Return the next name which partially matches from the file list. */
    while (list_index < entry_lenght && (name = subentries[list_index]))
        {
            char fullname[MAXPATH] = "";
            strncat(fullname, text, last_slash);
            strcat(fullname, "/");
            strcat(fullname, name);
            list_index++;
            if (strncmp (fullname, text, len) == 0)
                return (strdup(fullname));
            if (list_index == entry_lenght)
                break;
        }

    /* If no names matched, then return NULL. */
    return ((char *)NULL);
}

static void empty_it(){ return; }

static void disk_it() {
  verbose("disk_it %p -> %p\n", (void*) current_ctx, (void*) waiting_ctx);
  if (current_ctx && waiting_ctx){
    verbose("give cpu to waiting\n");
    waiting_ctx->next = current_ctx->next;
    current_ctx->next = waiting_ctx;
    waiting_ctx->state = ACTIVABLE;
    yield();
    verbose("disk it returned\n");
  }
}
static void
timer_it() {
  _out(TIMER_ALARM, 0xFFFFFFF0);
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

/* Look up NAME as the name of a command, and return a pointer to that
   command.  Return a NULL pointer if NAME isn't a command name. */
COMMAND* find_command(char* name) {
  register int i;

  for (i = 0; commands[i].name; i++)
    if (strcmp (name, commands[i].name) == 0)
      return (&commands[i]);

  return ((COMMAND *)NULL);
}

/* Execute a command line. */
int eval(char* line){
  register int i;
  COMMAND *command;
  char *word;

  /* Isolate the command word. */
  i = 0;
  while (line[i] && whitespace (line[i]))
    i++;
  word = line + i;

  while (line[i] && !whitespace (line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  command = find_command(word);

  if (!command)
    {
      fprintf (stderr, "%s: No such command.\n", word);
      return-1;
    }

  /* Get argument to command, if any. */
  while (whitespace (line[i]))
    i++;

  word = line + i;

  /* Call the function. */
  return ((*(command->func)) (word));
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
    fflush(stdin);
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
          create_ctx(32768, (void*) eval, cmdline_cpy + 1);
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

/* Set up hardware configuration
 */
int boot(){
  char *hw_config;
  int status, i;

  /* Hardware initialization */
  hw_config = "hardware.ini";
  status = init_hardware(hw_config);
  if (status == 0){
    printf("error in hardware initialization with %s\n", hw_config);
    exit(1);
  }
  printf("will bind interrupts\n");
  /* Interrupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;
  _out(TIMER_PARAM,128+64); /* reset + alarm on + 8 tick / alarm */
  _out(TIMER_ALARM,0xFFFFFFFD);  /* alarm at next tick (at 0xFFFFFFFF) */
  IRQVECTOR[TIMER_IRQ] = timer_it;
  IRQVECTOR[HDA_IRQ] = disk_it;
  /* We are ready, begin to catch interruptions */
  _mask(1);
  verbose("Binded timer interruptions\n");

  return 0;
}

int main(int argc, char** argv){
  /* Initialize history */
  using_history();

  if (argc > 1 && strcmp(argv[1], "-v") == 0){
    is_verbose = 1;
    printf("Set verbose flag\n");
  } else {
    is_verbose = 0;
    printf("Verbose flag not set. Use -v if you want to\n");
  }

  boot();
  verbose("Boot successfull\n");
  mount_volume(0);
  verbose("Volume 0 has been automatically mounted. Use ``mount`` to mount another\n");
  initialize_readline ();    /* Bind our completer. */
  printf("Welcome in shell. Build date %s %s\n", __DATE__, __TIME__);
  printf("Type ``help`` to find out all the available commands in this shell\n");
  printf("Add ``&`` in front of a command to run it in a new context (in background)\n");
  printf("\n");
  create_ctx(16000, shell_loop, NULL);
  yield();

  umount();
  fflush(stdout);
  exit(EXIT_SUCCESS);
}
