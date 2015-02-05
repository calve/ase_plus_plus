#ifndef __BUILTINS_H__
#define __BUILTINS_H__

typedef struct {
    char *name;                 /* User printable name of the function. */
    void *func;                 /* Function to call to do the job. */
    char *doc;                  /* Documentation for this function.  */
} COMMAND;

void do_help();
void do_cat(char* arguments);
void do_cd(char* arguments);
void do_compute(char* arguments);
void do_cp(char* arguments);
void do_ed(char* arguments);
void do_exit();
void do_ls(char* arguments);
void do_mkdir(char* arguments);
void do_mount(char* arguments);
void do_rm(char* arguments);

COMMAND commands[] = {
  { "cat", do_cat, "Print file FILE" },
  { "cd", do_cd, "Change to directory DIR" },
  { "compute", do_cd, "Compute stuffs DIR" },
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

#endif
