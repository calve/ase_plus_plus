#include "ctx.h"
#include "hw.h"
#include "sem.h"

/* ====================== Semaphore ====================== */

/* Initialisation du semaphore */
void sem_init(struct sem_s *sem, unsigned int val)
{
  sem->sem_magic_number=SEM_MAGIC_NUMBER;
  sem->sem_compteur=val;
  sem->sem_ctx_blocked=NULL;
}

/* Fonction wait des semaphores */
void sem_down(struct sem_s *sem)
{
  irq_disable();
  assert(sem->sem_magic_number==SEM_MAGIC_NUMBER);
  if (sem->sem_compteur > 0)
  {
    /* Take the semaphore and continue */
    sem->sem_compteur = sem->sem_compteur - 1;
    irq_enable();
    return;
  }
  else {
    /* Cannot take the semaphore now, give time to another process */
    current_ctx->state = BLOCKED;
    current_ctx->next_semaphore_context = sem->sem_ctx_blocked;
    sem->sem_ctx_blocked = current_ctx;
    irq_enable();
    yield();
    return;
  }
}

/* Fonction signal des semaphores */
void sem_up(struct sem_s *sem)
{
  struct ctx_s *ctx = NULL;
  irq_disable();
  assert(sem->sem_magic_number==SEM_MAGIC_NUMBER);
  sem->sem_compteur = sem->sem_compteur + 1;
  if (sem->sem_compteur >= 1)
  {
    assert(sem->sem_ctx_blocked);
    ctx = sem->sem_ctx_blocked;
    ctx->state = ACTIVABLE;
    sem->sem_ctx_blocked = ctx->next_semaphore_context;
    ctx->next_semaphore_context=NULL;
  }
  yield();
}

