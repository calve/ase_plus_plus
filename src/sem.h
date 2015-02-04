#ifndef __SEM__
#define __SEM__

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define SEM_MAGIC_NUMBER 0x00000069

struct sem_s {
	int sem_compteur;
	struct ctx_s* sem_ctx_blocked; /* Liste des contextes bloqués */
	int sem_magic_number;
};

void sem_init(struct sem_s *sem, unsigned int val);
void sem_down(struct sem_s *sem);
void sem_up(struct sem_s *sem);

#endif