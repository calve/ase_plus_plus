#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hardware_ini.h"
#include "../include/hardware.h"
#include "hw.h"
#include "ctx.h"

/* Use this constant to verify integrity of contexts */
#define CANARY 0xCAFEBABE

struct ctx_s *current_ctx[MAX_CORE] = {NULL};
static int first_context = 1;
int current_core = 0;

void switch_to_ctx(struct ctx_s *ctx);

/** Create a context with it's own stack and function
 * Create a struct to pass multiple arguments.
 * Example :
 *  void example() {
 *      struct {
 *              int cylinder;
 *              int sector;
 *              unsigned char *buffer;
 *              int size;
 *
 *      } arguments;
 *      arguments.cylinder = ncyl_of_nbloc(vol, nbloc);
 *      arguments.sector = nsec_of_nbloc(vol, nbloc);
 *      arguments.buffer = buffer;
 *      arguments.size = size;
 *      create_ctx(16000, (void*) read_nsector, (&arguments));
 * }
 */
struct ctx_s* create_ctx(int stack_size, func_t function, void *arguments){
  struct ctx_s *ctx;
  ctx = malloc(sizeof(struct ctx_s));
  ctx->state = ACTIVABLE;
  ctx->function = function;
  ctx->arguments = arguments;
  ctx->stack = malloc(stack_size);
  assert(ctx->stack);
  ctx->esp = (char*) ctx->stack + stack_size;
  ctx->ebp = ctx->esp;
  ctx->canary = CANARY;
  /* Here we should choose a core depending of the load of each ones
   */
  /* Keep the list ordered */
  if (current_ctx[current_core] == NULL)
    { /* First context created, initiate the list */
      ctx->next = ctx;
      current_ctx[current_core] = ctx;
    }
  else
    {
      ctx->next = current_ctx[current_core]->next;
      current_ctx[current_core]->next = ctx;
    }
  return ctx;
}

/** Delete the current context
 * We should use a temporary stack in this procedure, so that there would be no problem when free(current_ctx[current_core]->stack)
 */
void delete_ctx(){
  struct ctx_s* iterator = current_ctx[current_core];
  void* stack = current_ctx[current_core]->stack;

  /* Found which context previous the current context */
  while (iterator->next != current_ctx[current_core]){
    iterator = iterator->next;
  }
  if (current_ctx[current_core] == current_ctx[current_core]->next){
    fprintf(stderr,"\nno more context, terminating\n");
    exit(0);
  }
  iterator->next = current_ctx[current_core]->next;
  /* at this point, current_ctx[current_core] is not in the linked list anymore */
  free(current_ctx[current_core]);
  free(stack);
}

/* Switch from one context to another
 */
void switch_to_ctx (struct ctx_s *ctx){
  irq_disable();
  assert(ctx);
  if (current_ctx[current_core]){
    asm("movl %%esp, %0"
        :"=r" (current_ctx[current_core]->esp));
    asm("movl %%ebp, %0"
        :"=r" (current_ctx[current_core]->ebp));
  }
  assert(ctx->canary == CANARY);
  assert(ctx->state != BLOCKED);
  current_ctx[current_core] = ctx;
  asm("movl %0, %%esp"
      :
      :"r" (current_ctx[current_core]->esp));
  asm("movl %0, %%ebp"
      :
      :"r" (current_ctx[current_core]->ebp));
  if (current_ctx[current_core]->state == ACTIVABLE){
    current_ctx[current_core]->state = READY;
    irq_enable();   /* can reenable an inq_disable from sem_down */
    current_ctx[current_core]->function(current_ctx[current_core]->arguments);
    /* Hey, function returned, we may remove current_ctx[current_core] from the list */
    current_ctx[current_core]->state = TERMINATED;
    delete_ctx();
    yield();
  }
  irq_enable();   /* can reenable an inq_disable from sem_down */
}


void yield(){
  if (current_ctx[current_core]){
    if (first_context == 1)
      {
        first_context = 0;
        switch_to_ctx(current_ctx[current_core]);
      }
    else
      {
        struct ctx_s *iterator = current_ctx[current_core]->next;
        while (iterator->state == BLOCKED){
          iterator = iterator->next;
        }
        if (iterator != current_ctx[current_core]){
          switch_to_ctx(iterator);
        }
      }
  }
}


/* Sleep for at least ``seconds`` seconds
 * At the moment, counting using TIMER_CLOCK seems buggy, so we just count to 2^30 ``seconds`` times
 */
void second_sleep(int seconds){
  /* Wait for i seconds */
  for (int i = 0; i < seconds; i++){
    int count = 0;
    while (1){
      count++;
      if (count > 1<<30)
        break;
    }
  }
  return;
}
