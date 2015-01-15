#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#include "mbr.h"

void print_help(char* program_name){
  printf("usage %s --cylinder first_cylinder --sector first_sector --size block_size --type volume_type --name volume_name --serial\n", program_name);
}

int main(int argc, char** argv){
  int vol_type = -1;
  int sector = -1;
  int cylinder = -1;
  int size = -1;
  int opt = 0;
  int serial = 0;
  char* input_voltype="";
  char* volume_name="";
  int long_index =0;

  /* Specifying the expected options */
  static struct option long_options[] = {
    {"cylinder", required_argument, 0, 'c' },
    {"help", no_argument, 0, 'h' },
    {"name", required_argument, 0, 'N' },
    {"sector", required_argument, 0, 's' },
    {"serial", required_argument, 0, 'a' },
    {"size", required_argument, 0, 'S' },
    {"type", required_argument, 0, 't' },
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
        fprintf(stderr, "error - argument sector not an integer\n");
        exit(2);
      }
      sector = atoi(optarg);
      break;
    case 'c' :
      if (sscanf (optarg, "%i", &intvar)!=1){
        fprintf(stderr, "error - argument cylinder not an integer\n");
        exit(2);
      }
      cylinder = atoi(optarg);
      break;
    case 'S' :
      if (sscanf (optarg, "%i", &intvar)!=1){
        fprintf(stderr, "error - argument size not an integer\n");
        exit(2);
      }
      size = atoi(optarg);
      break;
    case 't' :
      input_voltype = optarg;
      if (strcmp(input_voltype, "base") == 0){
        vol_type = VOL_BASE;
      }
      else if (strcmp(input_voltype, "annex") == 0){
        vol_type = VOL_AUX;
      }
      else if (strcmp(input_voltype, "other") == 0){
        vol_type = VOL_OTHER;
      }
      else {
        fprintf(stderr, "Could not parse volume type\n");
        exit(2);
      }
      break;
    case 'N' :
      volume_name = optarg;
      break;
    case 'a':
      if (sscanf (optarg, "%i", &intvar)!=1){
        fprintf(stderr, "error - argument serial not an integer\n");
        exit(2);
      }
      serial = atoi(optarg);
      break;
    default:
      print_help(argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  init_mbr();

  if (cylinder < 0)
    {
      printf("Saisir le 1er cylindre:\n");
      scanf("%i", &cylinder);
    }
  if (sector < 0)
    {
      printf("Saisir le 1er secteur:\n");
      scanf("%i", &sector);
    }
  if (size < 0)
    {
      printf("Saisir le nombre de secteurs:\n");
      scanf("%i", &size);
    }
  if (vol_type < 0)
    {
      printf("Choisir le type de volume (0- Principal\n1- Etendu\n2- Autre:\n");
      scanf("%i", &vol_type);
    }
  create_volume(cylinder, sector, size, vol_type);
  list_volumes();
  write_mbr();
  exit(EXIT_SUCCESS);
}
