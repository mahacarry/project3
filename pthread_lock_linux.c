#include <stdio.h>
#include <pthread.h>

int shared_resource = 0;

#define NUM_ITERS 30
#define NUM_THREADS 30

int choosing[NUM_THREADS];
int number[NUM_THREADS];

//Lamport algorithm
void lock(int tid)
{	
	choosing[tid] = 1;		//true

    	int max = 0;
    	for(int i = 0; i < NUM_THREADS; i++) {
        	if(number[i] > max) max = number[i];
    	}	
    	number[tid] = max + 1;
    	choosing[tid] = 0;		//false

    for(int j = 0; j < NUM_THREADS; j++) {
        while(choosing[j]) {

	}
        while(number[j] != 0 && (number[j] < number[tid] || (number[j] == number[tid] && j < tid))) {
            
        }
    }
}

void unlock(int tid)
{
	number[tid] = 0;
}

void* thread_func(void* arg) {
    int tid = *(int*)arg;
    
    lock(tid);
    
        for(int i = 0; i < NUM_ITERS; i++)		shared_resource++;
    
    unlock(tid);
    
    pthread_exit(NULL);
}

int main() {

    int n = NUM_THREADS;
    pthread_t threads[n];
    int tids[n];
    
    for(int i = 0; i < n; i++){
	choosing[i] = 0;
        number[i] = 0;
    }

    for(int i = 0; i < n; i++){
        tids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &tids[i]);
    }
    
    for(int i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }

    printf("shared: %d\n", shared_resource);
    
    return 0;
}
