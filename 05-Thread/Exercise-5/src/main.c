#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUMBER_READERS  5
#define NUM_WRITERS     2
#define NUM_WRITES      5 

int data = 0; // Shared data

// Read-write lock 
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* read_thread(void *arg) {

}

void* write_thread(void *arg) {

}

int main(int argc, char const *argv[])
{
    pthread_t readers[NUMBER_READERS], writers[NUM_WRITERS];
    int reader_ids[NUMBER_READERS], writer_ids[NUM_WRITES];

    // Initalize read-write lock
    pthread_rwlock_init(&rwlock, NULL);

    // Create reader threads
    for (int i = 0; i < NUMBER_READERS; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, read_thread, &reader_ids[i]);
    }

    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + 1;
        pthread_create(&writers[i], NULL, write_thread, &writer_ids[i]);
    }

    // Wait for all thread to finish
    for (int i = 0; i < NUMBER_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    // Destroy read-write lock
    pthread_rwlock_destroy(&rwlock);

    printf("\nFinal value of data: %d\n", data);

    return 0;
}
