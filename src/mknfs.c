#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#include "tools.h"
#include "mbr.h"
#include "superbloc.h"

void print_help(char* program_name){
    printf("usage %s --volume volume --serial serial --size block_size\n", program_name);
}

int volume_loop()
{
    int vol;
    printf("Saisir le numero du volume:\n");
    while (1)
        {
            if (fscanf (stdin, "%i", &vol)!=1){ /* Parse one integer only */
                fprintf(stderr, "error - volume must be an integer\n");
                exit(2);
            }
            if (vol > MAXVOL || mbr.mbr_vol[vol].vol_nsectors == 0){
                printf("Veuillez saisir un  numero de volume valide:\n");
            }
            else break;
        }
    return vol;
}

int main(int argc, char** argv)
{
    int vol = -1;
    int serial = -1;
    int nbblocs = -1;
    int opt = 0;
    int long_index = 0;
    char test[32] = "test";

    /* Specifying the expected options */
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h' },
        {"serial", required_argument, 0, 's' },
        {"size", required_argument, 0, 'l' },
        {"volume", required_argument, 0, 'v' },
        {0, 0, 0, 0 }
    };

    /* Parsing options */
    opterr = 0;

    while ((opt = getopt_long(argc, argv, "c:s:S:t:N:a:", long_options, &long_index )) != -1) {
        switch (opt) {
            int intvar;
        case 'h' :
            print_help(argv[0]);
            exit(0);
            break;
        case 's' :
            if (sscanf (optarg, "%i", &intvar)!=1){
                fprintf(stderr, "error - argument size not an integer\n");
                exit(2);
            }
            serial = atoi(optarg);
            break;
        case 'l' :
            if (sscanf (optarg, "%i", &intvar)!=1){
                fprintf(stderr, "error - argument size not an integer\n");
                exit(2);
            }
            nbblocs = atoi(optarg);
            break;
        case 'v' :
            if (sscanf (optarg, "%i", &intvar)!=1){
                fprintf(stderr, "error - argument volume not an integer\n");
                exit(2);
            }
            vol = atoi(optarg);
            break;
        default:
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    init_mbr();
    if (vol < 0 || serial < 0 || nbblocs < 0)
      {
        list_volumes();
      }
    /* write_mbr(); */ /* What the point ? */

    if (vol < 0){
        vol = volume_loop();
    }
    if (serial < 0){
        printf("Saisir le numero de série:\n");
        scanf("%i", &serial);
    }
    if (nbblocs < 0){
        printf("Saisir le nombre de blocs:\n");
        scanf("%i", &nbblocs);

    }
    format_vol(vol);
    printf("Formated\n");
    init_super(vol, serial, test);

    printf("Taux d'occupation : %f/100\n", taux_occupation(vol));

    exit(EXIT_SUCCESS);
}
