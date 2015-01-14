#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbr.h"

int main()
{
	int cylinder;
    int sector;
    int nbsector;
    int i;

    init_mbr();

    printf("Saisir le 1er cylindre:\n");
    scanf("%i", &cylinder);
    printf("Saisir le 1er secteur:\n");
    scanf("%i", &sector);
    printf("Saisir le nombre de secteurs:\n");
    scanf("%i", &nbsector);
    printf("Choisir le type de volume (0- Principal\n1- Etendu\n2- Autre:\n");
    scanf("%i", &i);
          
    create_volume(sector, cylinder, nbsector, i);
	list_volumes();
    write_mbr();
    exit(EXIT_SUCCESS);
}