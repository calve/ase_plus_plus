#ifndef __BUILTINS_H__
#define __BUILTINS_H__
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>  /* Needed for rl_icpfunc_t */

typedef struct {
    char *name;                 /* User printable name of the function. */
    rl_icpfunc_t *func;                 /* Function to call to do the job. */
    char *doc;                  /* Documentation for this function.  */
} COMMAND;

int do_help(char* arguments);
int do_cat(char* arguments);
int do_cd(char* arguments);
int do_clock(char* arguments);
int do_compute(char* arguments);
int do_cp(char* arguments);
int do_ed(char* arguments);
int do_exit(char* arguments);
int do_ls(char* arguments);
int do_mkdir(char* arguments);
int do_mount(char* arguments);
int do_rm(char* arguments);

extern COMMAND commands[];

#endif
