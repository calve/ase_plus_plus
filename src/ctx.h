#ifndef __ctx_h__
#define __ctx_h__
#include "hardware_ini.h"

typedef void (func_t) (void*);

enum state_e {
  ACTIVABLE,
  BLOCKED,
  READY,
  RUNNING,
  TERMINATED
};


typedef struct ctx_s {
  enum state_e state;
  func_t *function;
  int blocked;
  struct ctx_s *next;
  struct ctx_s *next_semaphore_context;  /* may be next */
  unsigned int canary;
  void *arguments;
  void *ebp;
  void *esp;
  void *stack;
} ctx_t;


/* Some functions we will need */
void yield();
struct ctx_s* create_ctx(int stack_size, func_t function, void *arguments);
void delete_ctx();
struct ctx_s *current_ctx[MAX_CORE];
int current_core;

#endif
