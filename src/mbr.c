#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware_ini.h"
#include "../include/hardware.h"
#include "drive.h"
#include "mbr.h"
#include "sem.h"

struct mbr_s mbr;
struct sem_s mbr_mutex;

static void
empty_it()
{
    return;
}

void read_mbr()
{
	unsigned char buff[HDA_SECTORSIZE];
	read_sector(0,0,buff);
	memcpy(&mbr, buff, sizeof(struct mbr_s)); /* mbr = *((struct mbr_s*)buffer);*/
        sem_init(&mbr_mutex, 1);
	if (mbr.magic_number != MBR_MAGIC)
	{
                printf("init the mbr\n");
		/* Initialisation */
		mbr.nbvols = 0;
		memset(&mbr.mbr_vol, 0, sizeof(struct vol_s)*MAXVOL);
		mbr.magic_number = MBR_MAGIC;
	}
}

void write_mbr ()
{
	unsigned char buffer[HDA_SECTORSIZE];
	assert (mbr.magic_number == MBR_MAGIC);
	memset(buffer, 0, HDA_SECTORSIZE);
	memcpy(buffer, &mbr, sizeof(struct mbr_s));
	write_sector(0, 0, buffer);
}

/* ------------ A COMPLETER ------------ */
/* Retourne le numero de secteur du bloc */
int nsec_of_nbloc(unsigned int nvol, unsigned int nbloc)
{
        assert(mbr.magic_number == MBR_MAGIC);
        assert(nvol < MAXVOL);
        return (mbr.mbr_vol[nvol].vol_sector + nbloc) % MAX_SECTOR;
}

/* Retourne le numéro du cylindre du bloc */
int ncyl_of_nbloc(unsigned int nvol, unsigned int nbloc)
{
        assert(mbr.magic_number == MBR_MAGIC);
        assert(nvol < MAXVOL);
        return mbr.mbr_vol[nvol].vol_cylinder + ((mbr.mbr_vol[nvol].vol_sector + nbloc) / MAX_SECTOR);
}

void read_bloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer)
{
	int cylinder = ncyl_of_nbloc(vol, nbloc);
	int sector = nsec_of_nbloc(vol, nbloc);
        sem_down(&mbr_mutex);
	read_sector(cylinder, sector, buffer);
        sem_up(&mbr_mutex);
}


void read_nbloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer, int size)
{
	int cylinder = ncyl_of_nbloc(vol, nbloc);
	int sector = nsec_of_nbloc(vol, nbloc);
        sem_down(&mbr_mutex);
	read_nsector(cylinder, sector, buffer, size);
        sem_up(&mbr_mutex);
}


void write_bloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer)
{
	int cylinder = ncyl_of_nbloc(vol, nbloc);
	int sector = nsec_of_nbloc(vol, nbloc);
        sem_down(&mbr_mutex);
	write_sector(cylinder, sector, buffer);
        sem_up(&mbr_mutex);
}

void write_nbloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer, int size)
{
	int cylinder = ncyl_of_nbloc(vol, nbloc);
	int sector = nsec_of_nbloc(vol, nbloc);
        sem_down(&mbr_mutex);
	write_nsector(cylinder, sector, buffer, size);
        sem_up(&mbr_mutex);
}

/* Format all sectors of one volume
 */
void format_vol(unsigned int vol)
{
	int nsector = mbr.mbr_vol[vol].vol_nsectors;
        unsigned int i;
        sem_down(&mbr_mutex);
        for (i = 0; i < mbr.mbr_vol[vol].vol_nsectors; i++)
          {
            int cylinder = ncyl_of_nbloc(vol, i);
            int sector = nsec_of_nbloc(vol, i);
            format_sector(cylinder, sector, nsector, 0);
          }
        sem_up(&mbr_mutex);
}

char* display_type_vol(enum type_vol_e type)
{
	switch(type){
		case VOL_BASE: return "Principale";
		case VOL_AUX: return "Etendue";
		default: return "Autre";
	}
}

void list_volumes()
{
	int i;
	printf("Volume|Cylindre|Secteur|Nb secteurs|Type\n-------------------------------------------------\n");
	for (i=0; i<mbr.nbvols; i++){
		printf("sda%d\t%d\t%d\t%d\t%s\n", 
			i,
			mbr.mbr_vol[i].vol_cylinder,
			mbr.mbr_vol[i].vol_sector,
			mbr.mbr_vol[i].vol_nsectors,
			display_type_vol(mbr.mbr_vol[i].vol_type));
	}
}

void create_volume(unsigned int cylinder, 
				unsigned int sector, 
				unsigned int nsectors, 
				enum type_vol_e type)
{
	if (mbr.nbvols == MAXVOL)
	{
		fprintf(stderr, "Impossible de créer un nouveau volume car le quota est dépassé\n");
	} 
	else
	{
		struct vol_s new;
		new.vol_cylinder = cylinder;
		new.vol_sector = sector;
		new.vol_nsectors = nsectors;
		new.vol_type = type;
		new.valide = 0;

		mbr.mbr_vol[mbr.nbvols++] = new;
	}
}

void delete_volume(unsigned int number)
{
	if (number >= mbr.nbvols  || number < 0){
		fprintf(stderr, "Impossible de supprimer le volume %d car ce dernier n'existe pas\n", number);
	}
	else
	{
		int i;
		/* On remet a jour les volumes restant */
		for (i = number+1; i<=mbr.nbvols; i++)
		{
			mbr.mbr_vol[i-1] = mbr.mbr_vol[i];
		}
		mbr.nbvols--;
	}
}	

void init_mbr()
{
	int i;
    /* init hardware */
    if(init_hardware("hardware.ini") == 0) 
    {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

    /* Interreupt handlers */
    for(i=0; i<16; i++)
		IRQVECTOR[i] = empty_it;

	read_mbr();
}
