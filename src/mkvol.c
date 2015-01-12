#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#include "block.h"
#include "vol.h"


static void
empty_it()
{
  return;
}


int main(int argc, char** argv){
  int i;
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
    {"sector", required_argument, 0, 's' },
    {"size", required_argument, 0, 'S' },
    {"type", required_argument, 0, 't' },
    {"name", required_argument, 0, 'N' },
    {"serial", required_argument, 0, 'a' },
    {0, 0, 0, 0 }
  };

  /* Parsing options */
  opterr = 0;

  while ((opt = getopt_long(argc, argv, "c:s:S:t:N:a:", long_options, &long_index )) != -1) {
    switch (opt) {
      int intvar;
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
        vol_type = VOLTYPE_BASE;
      }
      else if (strcmp(input_voltype, "annex") == 0){
        vol_type = VOLTYPE_ANNEX;
      }
      else if (strcmp(input_voltype, "other") == 0){
        vol_type = VOLTYPE_OTHER;
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
      printf("usage %s --cylinder first_cylinder --sector first_sector --size block_size --type volume_type --name volume_name --serial", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if(init_hardware("hardware.ini") == 0) {
    fprintf(stderr, "Error in hardware initialization\n");
    exit(EXIT_FAILURE);
  }

  /* Interrupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = empty_it;

  /* Allows all IT */
  _mask(1);
  load_mbr();
  mkvol(cylinder, sector, size, vol_type);
  init_super(volume_name, serial);
  return 0;
}
