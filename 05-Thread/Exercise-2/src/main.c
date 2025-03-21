#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS         3           // Number of thread to be created
#define NUM_INTERATIONS     1000000     // Number of increments per thread

long long counter = 0;  // Global counter variable

pthread_mutex_t mutex;

// Thread function prototype
void* thread_counter(void* arg);

int main(int argc, char const *argv[])
{
    pthread_t threads[NUM_THREADS];
    int ret;

    // Initialize the mutex before using it
    pthread_mutex_init(&mutex, NULL);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        ret = pthread_create(&threads[i], NULL, thread_counter, NULL);
        if (ret) {
            printf("Error creating thread %d: %d\n", i + 1, ret);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to complete using pthread_join
    for (int i = 0; i < NUM_THREADS; i++) {
        ret = pthread_join(threads[i], NULL);

        // Check if joining the thread
        if(ret) {
            printf("Error joining thread %d: %d\n", i + 1, ret);
            exit(EXIT_FAILURE);
        }
    }
    // Print the final value of the counter after all threads have completed
    printf("Final counter value: %lld\n", counter);

    // Destroy the mutex to release resources
    pthread_mutex_destroy(&mutex);

    return 0;
}

// Thread function to increment the counter
void* thread_counter(void* arg) {
   for (int i = 0; i < NUM_INTERATIONS; i++) {
        // Lock the mutex when access the counter
        pthread_mutex_lock(&mutex);
        counter++;
        // Unlock the mutex to allow other threads to access the counter
        pthread_mutex_unlock(&mutex);
   }
   pthread_exit(NULL);
}