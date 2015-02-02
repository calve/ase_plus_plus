#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons.h"

int is_verbose = 0;
char cwd[MAXPROMPT] = "/";

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
