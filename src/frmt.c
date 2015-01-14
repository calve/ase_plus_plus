#include "drive.h"

static void
empty_it()
{
    return;
}

int
main(int argc, char **argv)
{
    unsigned int i, j, sectorsize, sector, cylinder, nbsector;
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

    if (argc == 4)
    {
        cylinder = (unsigned int)atoi(argv[1]);
        sector = (unsigned int)atoi(argv[2]);
        nbsector = (unsigned int)atoi(argv[3]);

        _out(HDA_CMDREG,CMD_DSKINFO);
        sectorsize=_in(HDA_DATAREGS+4)<<8;
    	sectorsize+=_in(HDA_DATAREGS+5);
    	buff=malloc(sectorsize);
        for (j=0; j < nbsector; j++)
        {
    	    read_sector(cylinder,sector,buff);
            format_sector(cylinder,sector++,1,0);
            dump(buff,sectorsize,0,1);
        }
        printf("\nLes données de %d secteurs du cylindre %d ont été formatées\n", nbsector, cylinder);
    }
    else 
    {
        printf("Utilisation : ./frmt [numero cylindre] [numero secteur] [nombre secteurs]\n");
    }

    /* and exit! */
    exit(EXIT_SUCCESS);
}