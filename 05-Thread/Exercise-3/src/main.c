#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUMBER_INTERATIONS 10   // Number of producer - consumer iterations

// Global variables
int data = 0;
int data_ready = 0;

// Mutex and condition variable
pthread_mutex_t mutex;
pthread_cond_t cond;

// Thread function prototype
void* producer(void *arg);
void* consumer(void *arg);

int main(int argc, char const *argv[])
{
    pthread_t pro_thread, cons_thread;
    int ret;

    // Initialize the random number generator
    srand(time(NULL));

    // Initialize the mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Create the producer thread
    ret = pthread_create(&pro_thread, NULL, producer, NULL);
    if(ret) {
        perror("Error creating producer thread\n");
        exit(EXIT_FAILURE);
    }
    
    // Create the consumer thread
    ret = pthread_create(&cons_thread, NULL, consumer, NULL);
    if(ret) {
        perror("Error creating consumer thread\n");
        exit(EXIT_FAILURE);
    }

    // Wait for both threads to finish
    pthread_join(pro_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Destroy the mutex and condition variable.
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}


void* producer(void *arg) {

}

void* consumer(void *arg) {

}
