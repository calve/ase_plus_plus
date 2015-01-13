#include "mbr.h"
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mbr_s mbr;

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
	if (mbr.magic_number != MBR_MAGIC)
	{
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
	int premierSec;
	premierSec = mbr.mbr_vol[nvol].vol_sector;
	return (premierSec + nbloc) % HDA_MAXSECTOR;
}

/* Retourne le numéro du cylindre du bloc */
int ncyl_of_nbloc(unsigned int nvol, unsigned int nbloc)
{
	int premierCyl = mbr.mbr_vol[nvol].vol_cylinder;
	int premierSec = nsec_of_nbloc(nvol, nbloc);
	return premierCyl + (premierSec + nbloc) / HDA_MAXSECTOR;
}

void read_bloc(unsigned int vol, unsigned int nbloc, unsigned char *buffer)
{
	int cylinder = ncyl_of_nbloc(vol, nbloc);
	int sector = nsec_of_nbloc(vol, nbloc);
	read_sector(cylinder, sector, buffer);
}

void write_bloc(unsigned int vol, unsigned int nbloc, const unsigned char *buffer)
{
	int cylinder = ncyl_of_nbloc(vol, nbloc);
	int sector = nsec_of_nbloc(vol, nbloc);
	write_sector(cylinder, sector, buffer);
}

void format_vol(unsigned int vol)
{
	int cylinder = mbr.mbr_vol[vol].vol_cylinder;
	int sector = mbr.mbr_vol[vol].vol_sector;
	int nsector = mbr.mbr_vol[vol].vol_nsectors;
	format_sector(cylinder, sector, nsector, 0);
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