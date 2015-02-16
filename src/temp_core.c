#include <stdlib.h>
#include <stdio.h>

#include "../include/hardware.h"
#include "hardware_ini.h"

#define CORE_CONFIG "core.ini"
#define HW_CONFIG "core.ini"

#define CORE_ID 0x126
#define CORE_STATUS 0x80
#define CORE_IRQMAPPER 0x82
#define TIMER_IRQ 2
#define TIMER_CLOCK 0xF0
#define TIMER_PARAM 0xF4
#define TIMER_ALARM 0xF8
#define CORE_LOCK	0x98
#define CORE_UNLOCK	0x99

void empty_handler() {

}

void init_handler() {
	int core_id = _in(CORE_ID);
	unsigned int i, j;

	printf("Start core %d...\n", core_id);

	_mask(0x0);

	while(1) {

		printf("Lock core %d\n", core_id);

		while(_in(CORE_LOCK) != 0x1) {
			/*printf("verrou locked\n");*/
		}

		for (j = 0 ; j < 10 ; j++) {
			printf("--> core%d\n", core_id);
		}

		_out(CORE_UNLOCK, 0x0);
		printf("Unlock core %d\n", core_id);
	}
}

void timer_handler() {
	int core_id = _in(CORE_ID);

	printf("Timer IRQ in core %d\n", core_id);

	_out(TIMER_ALARM,0xFFFFFFFD);
}

int main() {
	unsigned int i;
	printf("hardware %d\n", init_hardware(CORE_CONFIG));

	for(i=0; i<16; i++)
		IRQVECTOR[i] = empty_handler;

	IRQVECTOR[0] = init_handler;
	IRQVECTOR[TIMER_IRQ] = timer_handler;
	_out(TIMER_PARAM,128+64);
	_out(TIMER_ALARM,0xFFFFFFFE);

	_out(CORE_STATUS, 0x3);
	/*_out(CORE_IRQMAPPER+0, 1 << TIMER_IRQ);*/
	_out(CORE_IRQMAPPER+1, 1 << TIMER_IRQ);

	init_handler();

	return EXIT_SUCCESS;
}
