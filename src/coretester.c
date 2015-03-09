#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/hardware.h"
#include "hardware_ini.h"
#include "commons.h"

#define CORE_CONFIG "core.ini"
#define HW_CONFIG "core.ini"

#define CORE_STATUS		0x80
#define CORE_IRQMAPPER	0x82

void core(){
    while(1){
        /*second_sleep(2);*/
        printf(" --> core%d\n",_in(CORE_ID));
    }
}


static void
empty_it()
{
    return;
}

static void
timer_it() {
    printf(" --> interrupt%d\n",_in(CORE_ID));
    _out(TIMER_ALARM, 0xFFFFFFFD);
}


int main(int argc, char** argv){

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
    _mask(0x0);

    IRQVECTOR[0]=core;
    IRQVECTOR[TIMER_IRQ] = timer_it;
    _out(TIMER_PARAM,128+64+32+8); /* reset + alarm on + 8 tick / alarm */
    _out(TIMER_ALARM,0xFFFFFFE);  /* alarm at next tick (at 0xFFFFFFFF) */


    _out(CORE_STATUS, 0x3);
    for (i=0; i<2; i++)
        _out(CORE_IRQMAPPER+i, 0x0);
    _out(CORE_IRQMAPPER, 1<< TIMER_IRQ);
    _out(CORE_IRQMAPPER+1, 1<< TIMER_IRQ);

    core();

    exit(EXIT_SUCCESS);
}
