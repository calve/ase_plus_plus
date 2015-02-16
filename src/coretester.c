#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/hardware.h"

#define CORE_CONFIG "core.ini"
#define HW_CONFIG "core.ini"

#define CORE_STATUS		0x80
#define CORE_IRQMAPPER	0x82   

void core(){
	while(1)
		printf(" --> core%d\n",_in(0x126));
}

void interrupt(){
	printf(" --> interrupt%d\n",_in(0x126));
}


static void
empty_it()
{
    return;
}


void main(int argc, char** argv){

	int i;
    /* init hardware */
    if(init_hardware("etc/core.ini") == 0) 
    {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

    /* Interreupt handlers */
    for(i=0; i<16; i++)
	IRQVECTOR[i] = empty_it;

    /* Allows all IT */
    _mask(1);
    
    IRQVECTOR[3]=interrupt;
	IRQVECTOR[0]=core;
	_out(CORE_STATUS, 0x7);
	_out(CORE_IRQMAPPER, 0x0);
	_out(CORE_IRQMAPPER+1, 0x8);
	core(0);
}
