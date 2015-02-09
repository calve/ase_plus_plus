#include "sem.h"
#include "ctx.h"

#define MAX_BUFFER 5

struct sem_s mutex;
struct sem_s fill_count;
struct sem_s empty_count;
int buffer;

int producer(){
    int counter = 0;
    while(1) {
        sem_down(&empty_count);
        sem_down(&mutex);
        buffer = counter++;
        printf("produced %i\n", buffer);
        sem_up(&mutex);
        sem_up(&fill_count);
    }
}

int consummer(){
    while(1) {
        sem_down(&fill_count);
        sem_down(&mutex);
        printf("consume %i\n", buffer);
        buffer = 0;
        sem_up(&mutex);
        sem_up(&empty_count);
    }
}

int main(){
    struct ctx_s *prod_ctx, *cons_ctx;
    sem_init(&mutex, 1);
    sem_init(&fill_count, 0);
    sem_init(&empty_count, MAX_BUFFER);
    cons_ctx = create_ctx(16000, (void*) consummer, (void*) NULL);
    prod_ctx = create_ctx(16000, (void*) producer, (void*) NULL);
    yield();
    printf("hello world");
}
