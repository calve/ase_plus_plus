#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hardware_ini.h"
#include "../include/hardware.h"
#include "hw.h"
#include "ctx.h"

/* Use this constant to verify integrity of contexts */
#define CANARY 0xCAFEBABE

struct ctx_s *current_ctx = NULL;
static int first_context = 1;

void switch_to_ctx(struct ctx_s *ctx);
/** Create a context with it's own stack and function
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
  /* Keep the list ordered */
  if (current_ctx == NULL)
    { /* First context created, initiate the list */
      ctx->next = ctx;
      current_ctx = ctx;
    }
  else
    {
      ctx->next = current_ctx->next;
      current_ctx->next = ctx;
    }
  return ctx;
}

/** Delete the current context
 * We should use a temporary stack in this procedure, so that there would be no problem when free(current_ctx->stack)
 */
void delete_ctx(){
  struct ctx_s* iterator = current_ctx;
  void* stack = current_ctx->stack;

  /* Found which context previous the current context */
  while (iterator->next != current_ctx){
    iterator = iterator->next;
  }
  if (current_ctx == current_ctx->next){
    fprintf(stderr,"\nno more context, terminating\n");
    exit(0);
  }
  iterator->next = current_ctx->next;
  /* at this point, current_ctx is not in the linked list anymore */
  free(current_ctx);
  free(stack);
}

/* Switch from one context to another
 */
void switch_to_ctx (struct ctx_s *ctx){
  irq_disable();
  assert(ctx);
  if (current_ctx){
    asm("movl %%esp, %0"
        :"=r" (current_ctx->esp));
    asm("movl %%ebp, %0"
        :"=r" (current_ctx->ebp));
  }
  assert(ctx->canary == CANARY);
  assert(ctx->state != BLOCKED);
  current_ctx = ctx;
  asm("movl %0, %%esp"
      :
      :"r" (current_ctx->esp));
  asm("movl %0, %%ebp"
      :
      :"r" (current_ctx->ebp));
  if (current_ctx->state == ACTIVABLE){
    current_ctx->state = READY;
    irq_enable();   /* can reenable an inq_disable from sem_down */
    current_ctx->function(current_ctx->arguments);
    /* Hey, function returned, we may remove current_ctx from the list */
    current_ctx->state = TERMINATED;
    delete_ctx();
    yield();
  }
}


void yield(){
  if (current_ctx){
    if (first_context == 1)
      {
        first_context = 0;
        switch_to_ctx(current_ctx);
      }
    else
      {
        struct ctx_s *iterator = current_ctx->next;
        while (iterator->state == BLOCKED){
          iterator = iterator->next;
        }
        switch_to_ctx(iterator);
      }
  }
}

