#ifndef __COMMONS_H__
#define __COMMONS_H__
/* Defines commons variables and behaviours */

#define MAXPROMPT 256

/* This macro should be used like printf
 * it will output only if we are in verbose mode
 */
#define verbose(format, ...) do {                 \
    if (is_verbose)                               \
      fprintf(stdout, format, ##__VA_ARGS__);     \
  } while(0)
;

extern int is_verbose;
extern char cwd[MAXPROMPT]; /* Maybe it should not be global ... */

/* Construct the canonical path of ``path`` and store it in ``canonical``
 * returns a canonical path that starts at ``/`` and should not contains ``.`` (dot) or ``..`` (double dot)
 */
void canonical_path(char* canonical, char* path);

void second_sleep(int second);
#endif
