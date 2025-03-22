#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUMBER_PRODUCERS    10
#define NUMBER_CONSUMERS    5
#define BUFFER_SIZE         5

// Arrays to store thread IDs for producers and consumers
pthread_t tid_producers[NUMBER_PRODUCERS];
pthread_t tid_consumers[NUMBER_CONSUMERS];

// Mutex and condition variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

// Shared buffer and related variables
int buffer[BUFFER_SIZE];
int putptr = 0;
int takeptr = 0;
int count = 0;

// Producer function
void* producer(void *arg) {
    int loop = BUFFER_SIZE * 2;

    while (loop -- > 0) {
        // Generate a random value between 0 and 99
        int item = rand() % 100;
        // Lock mutex to protect shared resources
        pthread_mutex_lock(&mutex);

        // Wait for a signal from the consummer, if buffer is full
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&not_full, &mutex);
        }
        // Add value to the buffer
        buffer[putptr] = item;
        count++; // Increase the count of items in the buffer

        printf("Producer: %d\n", buffer[putptr]);
        // Update the put pointer
        putptr = (putptr + 1) % BUFFER_SIZE;

        // Signal that the buffer is not empty
        pthread_cond_signal(&not_empty);
        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

// Consumer function
void* consumer(void *arg) {
    int loop = BUFFER_SIZE * 4;

    while (loop -- > 0) {
        // Lock the mutex to protect shared resources
        pthread_mutex_lock(&mutex);

        // Wait for a signal from the producer, if buffer is empty
        while (count == 0) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        // Retrieve the item from the buffer
        int value = buffer[takeptr];
        buffer[takeptr] = -1;
        printf("\tConsumer: %d\n", value);

        // Update the take pointer
        takeptr = (takeptr + 1) % BUFFER_SIZE;
        count--; // Decrease the count of items in the 
        
        // Signal that the buffer is not full
        pthread_cond_signal(&not_full);
        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    // Create producer threads
    for (int i = 0; i < NUMBER_PRODUCERS; i++) {
        pthread_create(&tid_producers[i], NULL, producer, (void*)i);
    }

    // Create producer threads
    for (int i = 0; i < NUMBER_CONSUMERS; i++) {
        pthread_create(&tid_consumers[i], NULL, consumer, (void*)i);
    }

    // Wait for all producer threads to finish
    for (int i = 0; i < NUMBER_PRODUCERS; i++) {
        pthread_join(tid_producers[i], NULL);
    }
    
    // Wait for all consumer threads to finish
    for (int i = 0; i < NUMBER_CONSUMERS; i++) {
        pthread_join(tid_consumers[i], NULL);
    }

    return 0;
}
