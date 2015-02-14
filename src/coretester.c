#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/hardware.h"

#define CORE_CONFIG "core.ini"
#define HW_CONFIG "core.ini"

#define CORE_STATUS		0x80

void core(){
	while(1)
		printf(" --> core%d\n",_in(0x126));
}


static void
empty_it()
{
    return;
}


void main(int argc, char** argv){

	int i;
    /* init hardware */
    if(init_hardware(CORE_CONFIG) == 0)
    {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

    /* Interreupt handlers */
    for(i=0; i<16; i++)
	IRQVECTOR[i] = empty_it;

    /* Allows all IT */
    _mask(1);
    
    
	IRQVECTOR[0]=core;
	_out(CORE_STATUS, 0x7);
	core(0);
}
