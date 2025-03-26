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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

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
    if (ret) {
        perror("Error creating producer thread\n");
        exit(EXIT_FAILURE);
    }
    
    // Create the consumer thread
    ret = pthread_create(&cons_thread, NULL, consumer, NULL);
    if (ret) {
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

// Producer thread function
void* producer(void *arg) {
    for (int i = 0; i < NUMBER_INTERATIONS; i++) {
        // Lock the mutex before shared data
        pthread_mutex_lock(&mutex);

        // Generate a random number between 1 and 10
        data = rand() % 10 + 1; 

        printf("Producer: Generated data = %d\n", data);
        
        // Mark data ready
        data_ready = 1;
        
        // Send signal the consumer that data is available 
        pthread_cond_signal(&cond);

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    pthread_exit(NULL);
}

// Consumer thread function
void* consumer(void *arg) {
    int value[NUMBER_INTERATIONS]; // Array to store consumed values

    for (int i = 0; i < NUMBER_INTERATIONS; i++) {
        // Lock the mutex before accessing shared data
        pthread_mutex_lock(&mutex);
        
        // Wait until the producer signals that data is ready
        while (data_ready == 0) {
             pthread_cond_wait(&cond, &mutex);
        }

        value[i] = data;
        printf("Consumer: Consumed data = %d\n", data);

        // Reset the data ready flag
        data_ready = 0;

        // Unlock the mutex after consuming the data
        pthread_mutex_unlock(&mutex);
    }
    
    //Print all consumed values
    printf("\nAll values read:\n");
    for (int i = 0 ; i < NUMBER_INTERATIONS; i++) {
        printf("Value: %d\t", value[i]);
    }
    printf("\n");

    pthread_exit(NULL);
}
