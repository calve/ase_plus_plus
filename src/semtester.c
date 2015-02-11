#include "sem.h"
#include "ctx.h"
#include "../include/hardware.h"
#include "hardware_ini.h"
#include "commons.h"

#define MAX_BUFFER 5

struct sem_s mutex;
struct sem_s fill_count;
struct sem_s empty_count;
int head, tail =0;
int counter = 0;

void print_queue(int head, int tail){
  printf("\t\t(");
  for(int i = tail; i < head; i++){
    printf("%i, ", i+1);
  }
  printf(")\n");
}

int producerA(){
    while(1) {
        sem_down(&empty_count);
        sem_down(&mutex);
        head++;
        printf(" --> Alice produced %i", head);
        print_queue(head, tail);
        sem_up(&mutex);
        sem_up(&fill_count);
        second_sleep(1);
    }
}

int producerB(){
    while(1) {
        sem_down(&empty_count);
        sem_down(&mutex);
        head++;
        printf(" --> Bob produced %i", head);
        print_queue(head, tail);
        sem_up(&mutex);
        sem_up(&fill_count);
        second_sleep(1);
    }
}

int consummerC(){
    while(1) {
        sem_down(&fill_count);
        sem_down(&mutex);
        tail++;
        printf(" <-- Charlie consumed %i", tail);
        print_queue(head, tail);
        sem_up(&mutex);
        sem_up(&empty_count);
        second_sleep(2);
    }
}

int consummerD(){
    while(1) {
        sem_down(&fill_count);
        sem_down(&mutex);
        tail++;
        printf(" <-- Dona consumed %i", tail);
        print_queue(head, tail);
        sem_up(&mutex);
        sem_up(&empty_count);
        second_sleep(1);
    }
}

static void empty_it(){ return; }

static void
timer_it() {
  _out(TIMER_ALARM, 0xFFFFFFFE);
  yield();
}

int main(){
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
  create_ctx(16000, (void*) consummerD, (void*) NULL);
  create_ctx(16000, (void*) consummerC, (void*) NULL);
  create_ctx(16000, (void*) producerA, (void*) NULL);
  create_ctx(16000, (void*) producerB, (void*) NULL);
  yield();
  printf("hello world");
}
