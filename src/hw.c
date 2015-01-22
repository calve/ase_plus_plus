#include <stdio.h>
#include "hw.h"
#include "hardware_ini.h"
#include "../include/hardware.h"

void irq_enable(){
  _mask(1);
}

void irq_disable(){
  _mask(17);
}

void setup_irq(unsigned int irq, irq_handler_func_t handler){
  IRQVECTOR[irq] = handler;
}

void start_hw(){
  printf("started hardware\n");
}
