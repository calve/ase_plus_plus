#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"
#include "mbr.h"
#include "ifile.h"
#include "superbloc.h"

struct superbloc_s super;
unsigned current_volume;

void init_super(unsigned int vol, unsigned int num_serie, char nom[32])
{
        unsigned int i = 0;
        read_mbr();

	/* Initialisation du superbloc */
	super.magic = SUPER_MAGIC;
	super.numero = num_serie;
	strncpy(super.nom, nom, 32);
	super.id = 0;
	super.premier_libre = 1;
	super.nb_bloc_libre = 0;
	current_volume = vol;

        /* initialisation de la liste des blocs */
        for (i = 1; i < (mbr.mbr_vol[current_volume].vol_nsectors); i++){
            struct free_bloc_s block;
            struct free_bloc_s in_bloc;
            block.magic = BLOC_MAGIC;
            block.next_free = (i < mbr.mbr_vol[current_volume].vol_nsectors - 1 ? i+1 : BLOC_NULL);
            write_nbloc(current_volume, i, (unsigned char*)&block, sizeof(block));
            super.nb_bloc_libre++;

            /* Check bloc */
            read_nbloc(current_volume, i, (unsigned char*)&in_bloc, sizeof(in_bloc));
            assert(in_bloc.magic == BLOC_MAGIC);
        }
        save_super();
        assert(super.magic == SUPER_MAGIC);
}

int load_super(unsigned int vol)
{
	unsigned char buf[HDA_SECTORSIZE];

	current_volume = vol;

	read_bloc(vol, SUPER, buf);
	memcpy(&super, buf, sizeof(struct superbloc_s));
	if (super.magic != SUPER_MAGIC)
          {
            fprintf(stderr, "Superbloc corrupted\n");
            exit(2);
          }
	return 0;
}

void save_super()
{
    assert(super.magic == (int) SUPER_MAGIC);
    write_nbloc(current_volume, 0, (unsigned char*) &super, sizeof(super));
}

/* Returns an unallocated bloc from the superbloc bloc list
 */
unsigned int new_bloc()
{
    struct free_bloc_s free_bloc;
    int premier_libre;
    if (super.premier_libre == BLOC_NULL){
        printf("disk full\n");
        return 0;
    }
    premier_libre = super.premier_libre;
    read_nbloc(current_volume, premier_libre, (unsigned char*)&free_bloc, sizeof(free_bloc));
    assert(free_bloc.magic == BLOC_MAGIC);
    if (free_bloc.next_free != BLOC_NULL) {
        super.premier_libre = free_bloc.next_free;
    } else {
        super.premier_libre = BLOC_NULL;
    }
    super.nb_bloc_libre--;
    save_super();

    return premier_libre;
}

void free_bloc(unsigned int bloc)
{
	unsigned char buf[HDA_SECTORSIZE];
	struct free_bloc_s* fb;
	memset(buf, 0, HDA_SECTORSIZE);

	if(bloc==0){
		printf("free_bloc impossible sur le permier bloc (reserve)\n");
		return;
	}

	/* On bloque ici */
	read_bloc(current_volume, bloc, buf);
	fb = (struct free_bloc_s*) buf;
	fb->next_free = super.premier_libre;
	/* fb->size = 1; */
	write_bloc(current_volume, super.premier_libre, buf);
	super.premier_libre = bloc;
	super.nb_bloc_libre++;
	save_super();
}

float taux_occupation(unsigned int vol)
{
	return (1.00 - (float)super.nb_bloc_libre/(float)(mbr.mbr_vol[vol].vol_nsectors-1))*100.00;
}

void display()
{
	printf("Nombre de bloc : %d\nID\tTaille\tUtilisé\tDispo.\tPourcentage\n%d\t%d\t%d\t%d\t%f\n",
			mbr.mbr_vol[current_volume].vol_nsectors-1-super.nb_bloc_libre,
			super.id,
			mbr.mbr_vol[current_volume].vol_nsectors-1,
			mbr.mbr_vol[current_volume].vol_nsectors-1-super.nb_bloc_libre,
			super.nb_bloc_libre,
			taux_occupation(current_volume));
}
