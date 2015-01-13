#ifndef SB
#define SB

#include "mbr.h"

#define SUPER_MAGIC 0xACCD1001
#define SUPER 0

extern unsigned int current_vol;

struct free_bloc_s 
{
	unsigned size;
	unsigned next_free;
};

struct superbloc_s
{
	int magic;
	int numero;
	char nom[32];
	int id;
	unsigned premier_libre;
	unsigned nb_bloc_libre;
};

void init_super(unsigned int vol, unsigned int num_serie, char nom[32]);
int load_super(unsigned int vol);
void save_super();
unsigned int new_bloc();
void free_bloc(unsigned int bloc);
float taux_occupation(unsigned int vol);
void display();

#endif