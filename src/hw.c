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
