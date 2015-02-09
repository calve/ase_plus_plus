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
  sem->sem_compteur = sem->sem_compteur - 1;
  if (sem->sem_compteur < 0)
  {
    current_ctx->state = BLOCKED;
    current_ctx->next_semaphore_context = sem->sem_ctx_blocked;
    sem->sem_ctx_blocked = current_ctx;
    irq_enable();
    yield();
  }
  irq_enable();
}

/* Fonction signal des semaphores */
void sem_up(struct sem_s *sem)
{
  struct ctx_s *ctx = NULL;
  irq_disable();
  assert(sem->sem_magic_number==SEM_MAGIC_NUMBER);
  sem->sem_compteur = sem->sem_compteur + 1;
  if (sem->sem_compteur < 1)
  {
    assert(sem->sem_ctx_blocked);
    ctx = sem->sem_ctx_blocked;
    ctx->state = ACTIVABLE;
    sem->sem_ctx_blocked = ctx->next_semaphore_context;
    ctx->next_semaphore_context=NULL;
  }
  irq_enable();
}

