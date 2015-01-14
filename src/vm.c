/* ------------------------------
   Franquenouille Kevin
   Cornette Damien
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbr.h"


/* ------------------------------
   command list
   ------------------------------------------------------------*/
struct _cmd {
    char *name;
    void (*fun) (struct _cmd *c);
    char *comment;
};

static void list(struct _cmd *c);
static void new(struct _cmd *c);
static void del(struct _cmd *c);
static void help(struct _cmd *c) ;
static void save(struct _cmd *c);
static void quit(struct _cmd *c);
static void xit(struct _cmd *c);
static void none(struct _cmd *c) ;

static struct _cmd commands [] = {
    {"list", list, 	"display the partition table"},
    {"new",  new,	"create a new partition"},
    {"del",  del,	"delete a partition"},
    {"save", save,	"save the MBR"},
    {"quit", quit,	"save the MBR and quit"},
    {"exit", xit,	"exit (without saving)"},
    {"help", help,	"display this help"},
    {0, none, 		"unknown command, try help"}
} ;

/* ------------------------------
   dialog and execute 
   ------------------------------------------------------------*/

static void
execute(const char *name)
{
    struct _cmd *c = commands; 
  
    while (c->name && strcmp (name, c->name))
	c++;
    (*c->fun)(c);
}

static void
loop(void)
{
    char name[64];
    
    while (printf("> "), scanf("%62s", name) == 1)
	execute(name) ;
}

/* ------------------------------
   command execution 
   ------------------------------------------------------------*/
static void
list(struct _cmd *c)
{
    list_volumes();
}

static void
new(struct _cmd *c)
{
    int cylinder;
    int sector;
    int nbsector;
    int i;

    printf("Saisir le 1er cylindre:\n");
    scanf("%i", &cylinder);
    printf("Saisir le 1er secteur:\n");
    scanf("%i", &sector);
    printf("Saisir le nombre de secteurs:\n");
    scanf("%i", &nbsector);
    printf("Choisir le type de volume (0- Principal\n1- Etendu\n2- Autre:\n");
    scanf("%i", &i);
          
    create_volume(cylinder, sector, nbsector, i);
}

static void
del(struct _cmd *c)
{
    int number;

    printf("Saisir le volume à supprimer:\n");
    scanf("%i", &number);

    delete_volume(number);
}

static void
save(struct _cmd *c)
{
    write_mbr();
}

static void
quit(struct _cmd *c)
{
    int quitter;

    printf("Etes-vous sur de vouloir quitter ?\n1- Sauvegarder et quitter\n2- Quitter\n");
    scanf("%i", &quitter);

    if (quitter == 1)
    {
        write_mbr();
    }

    exit(EXIT_SUCCESS);
}

static void
do_xit()
{
    exit(EXIT_SUCCESS);
}

static void
xit(struct _cmd *dummy)
{
    do_xit(); 
}

static void
help(struct _cmd *dummy)
{
    struct _cmd *c = commands;
  
    for (; c->name; c++) 
	printf ("%s\t-- %s\n", c->name, c->comment);
}

static void
none(struct _cmd *c)
{
    printf ("%s\n", c->comment) ;
}

int
main(int argc, char **argv)
{
    /* Initialisation du mbr */
    init_mbr();

    /* dialog with user */ 
    loop();

    /* abnormal end of dialog (cause EOF for xample) */
    do_xit();

    /* make gcc -W happy */
    exit(EXIT_SUCCESS);
}
