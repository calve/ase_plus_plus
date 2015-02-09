#include "sem.h"
#include "ctx.h"
#include "../include/hardware.h"
#include "hardware_ini.h"
#include "commons.h"

#define MAX_BUFFER 5

struct sem_s mutex;
struct sem_s fill_count;
struct sem_s empty_count;
int buffer;
int counter = 0;

int producer(){
    while(1) {
        sem_down(&empty_count);
        sem_down(&mutex);
        second_sleep(1);
        buffer = counter++;
        printf(" --> produced %i\n", buffer);
        sem_up(&mutex);
        sem_up(&fill_count);
    }
}

int consummerA(){
    while(1) {
        sem_down(&fill_count);
        sem_down(&mutex);
        second_sleep(1);
        printf(" <-- consume-A %i\n", buffer);
        buffer--;
        sem_up(&mutex);
        sem_up(&empty_count);
    }
}

int consummerB(){
    while(1) {
        sem_down(&fill_count);
        sem_down(&mutex);
        second_sleep(3);
        printf(" <-- consume-B %i\n", buffer);
        buffer--;
        sem_up(&mutex);
        sem_up(&empty_count);
    }
}

static void empty_it(){ return; }

static void
timer_it() {
  _out(TIMER_ALARM, 0xFFFFFFFE);
  yield();
}

int main(){
    struct ctx_s *prod_ctx, *consA_ctx, *consB_ctx;
    char *hw_config;
    int status, i;

  /* Hardware initialization */
  hw_config = "hardware.ini";
  status = init_hardware(hw_config);
  if (status == 0){
      printf("error in hardware initialization with %s\n", hw_config);
      exit(1);
  }
  printf("will bind interrupts\n");
  /* Interrupt handlers */
  for(i=0; i<16; i++)
      IRQVECTOR[i] = empty_it;
  _out(TIMER_PARAM,128+64); /* reset + alarm on + 8 tick / alarm */
  _out(TIMER_ALARM,0xFFFFFFFD);  /* alarm at next tick (at 0xFFFFFFFF) */
  IRQVECTOR[TIMER_IRQ] = timer_it;
  
  /* We are ready, begin to catch interruptions */
  _mask(1);
  printf("Binded timer interruptions\n");
  sem_init(&mutex, 1);
  sem_init(&fill_count, 0);
  sem_init(&empty_count, MAX_BUFFER);
  consA_ctx = create_ctx(16000, (void*) consummerA, (void*) NULL);
  consB_ctx = create_ctx(16000, (void*) consummerB, (void*) NULL);
  prod_ctx = create_ctx(16000, (void*) producer, (void*) NULL);
  yield();
  printf("hello world");
}
