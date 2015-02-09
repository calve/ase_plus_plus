#include "sem.h"
#include "ctx.h"
#include "commons.h"
#include "../include/hardware.h"
#include "hardware_ini.h"

#define MAX_BUFFER 4

struct sem_s mutex;
struct sem_s fill_count;
struct sem_s empty_count;
int buffer = -1;
int counter = 0;

int producer(){
    while(1) {
        sem_down(&empty_count);
        sem_down(&mutex);
        printf(" --> produced %i\n", counter);
        buffer = counter++;
        sem_up(&mutex);
        sem_up(&fill_count);
    }
}

int consummer(){
    while(1) {
        int tmp;
        sem_down(&fill_count);
        sem_down(&mutex);
        printf(" <-- consume %i\n", buffer);
        tmp = buffer--;
        sem_up(&mutex);
        sem_up(&empty_count);
    }
}

int threadA(){
    while(1){
        sem_down(&mutex);
        printf("==== thread A starts\n");
        second_sleep(3);
        printf("==== thread A release\n");
        sem_up(&mutex);
    }
}

int threadB(){
    while(1){
        sem_down(&mutex);
        printf("=== thread B starts\n");
        second_sleep(3);
        printf("=== thread B release\n");
        sem_up(&mutex);
    }
}

static void empty_it(){ return; }

static void
timer_it() {
    _out(TIMER_ALARM, 0xFFFFFFFE);
    yield();
}

int main(){
    struct ctx_s *prod_ctx, *cons_ctx;
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

    cons_ctx = create_ctx(16000, (void*) threadA, (void*) NULL);
    prod_ctx = create_ctx(16000, (void*) threadB, (void*) NULL);

    printf("treadA %p threadB %p\n", (void*) threadA, (void*) threadB);
    yield();

}
