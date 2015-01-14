#include "drive.h"

static void
empty_it()
{
    return;
}

int
main(int argc, char **argv)
{
    unsigned int i, sectorsize, sector, cylinder;
    unsigned char* buff;
    
    /* init hardware */
    if(init_hardware("hardware.ini") == 0) 
    {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

    /* Interreupt handlers */
    for(i=0; i<16; i++)
	IRQVECTOR[i] = empty_it;

    /* Allows all IT */
    _mask(1);

    if (argc == 3)
    {
        cylinder = (unsigned int)atoi(argv[1]);
        sector = (unsigned int)atoi(argv[2]);
        
        _out(HDA_CMDREG,CMD_DSKINFO);
        sectorsize=_in(HDA_DATAREGS+4)<<8;
    	sectorsize+=_in(HDA_DATAREGS+5);
        buff=malloc(sectorsize);
        read_sector(cylinder,sector,buff);
    	dump(buff,sectorsize,0,1);
        printf("\nDonnées du cylindre %d et de secteur %d\n", cylinder, sector);
    } 
    else 
    {
        printf("Utilisation : ./dmps [numero cylindre] [numero secteur]\n");
    }

    /* and exit! */
    exit(EXIT_SUCCESS);
}