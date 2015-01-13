#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "superbloc.h"

int main() 
{
	int vol;
	int serie;
	int nbblocs;
	int i;
	char test[32] = "test";

	init_mbr();
	list_volumes();
    write_mbr();

    printf("Saisir le numero du volume:\n");
    scanf("%i", &vol);
    while (mbr.mbr_vol[vol].vol_nsectors == 0)
    {
    	printf("Veuillez saisir un  numero de volume valide:\n");
    	scanf("%i", &vol);
    }
    printf("Saisir le numero de série:\n");
    scanf("%i", &serie);
	init_super(vol, serie, test);
    
    printf("Saisir le nombre de blocs:\n");
    scanf("%i", &nbblocs);

	for (i=0; i<nbblocs; i++)
	{
		new_bloc();
	}

	save_super();
	if (nbblocs > mbr.mbr_vol[vol].vol_nsectors-1)
	{
		nbblocs = mbr.mbr_vol[vol].vol_nsectors-1;
	}
	printf("Superbloc rempli de %d blocs !\n", nbblocs);
	printf("Taux d'occupation : %f/100\n", taux_occupation(vol));

	exit(EXIT_SUCCESS);
}