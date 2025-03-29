#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUMBER_READERS  5    // Number of reader threads
#define NUM_WRITERS     2    // Number of writer threads
#define NUM_ITERATIONS  5    // Number of operations each thread will perform

int data = 0; // Shared data
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// Reader thread function
void* read_thread(void *arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_rwlock_rdlock(&rwlock);

        printf("Reader %d reads data: %d\n", thread_id, data);

        pthread_rwlock_unlock(&rwlock);

        usleep(100000);  // Simulate some processing
    }
    pthread_exit(NULL);
}
// Writer thread function
void* write_thread(void *arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_rwlock_wrlock(&rwlock);

        data++;  // Modify the shared 
        printf("Writer %d update data to: %d\n", thread_id, data);
        pthread_rwlock_unlock(&rwlock);

        usleep(200000);  // Simulate some processing
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t readers[NUMBER_READERS], writers[NUM_WRITERS];
    int reader_ids[NUMBER_READERS], writer_ids[NUM_WRITERS];

    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + 1;
        pthread_create(&writers[i], NULL, write_thread, &writer_ids[i]);
    }

    // Create reader thread 
    for (int i = 0; i < NUMBER_READERS; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, read_thread, &reader_ids[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < NUMBER_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    // Clean up and show the result
    pthread_rwlock_destroy(&rwlock);
    printf("\nFinal value of data: %d\n", data);
    
    return 0;
}
