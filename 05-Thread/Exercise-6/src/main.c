#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    // Library for POSIX threads
#include <time.h>      


#define ARRAY_SIZE 1000000  // Array size: 1 million elements
#define NUM_THREADS 4       // Number of threads to use

// Global variables (shared between threads)
int array[ARRAY_SIZE];     // Integer array to sum
long long total_sum = 0;   // Variable to store the total sum
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Initialize mutex to protect total_sum

// Data structure to pass information to each thread
typedef struct {
    int start;  
    int end;    
} ThreadData;

void* sum_array_portion(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    long long partial_sum = 0;
    
    // Calculate the sum of the assigned array portion
    for (int i = data->start; i < data->end; i++) {
        partial_sum += array[i];
    }
    
    // ----- CRITICAL SECTION BEGINS -----
    // Lock the mutex before accessing the shared variable total_sum
    pthread_mutex_lock(&mutex);
    
    // Add the local sum to the global sum (shared between threads)
    total_sum += partial_sum;
    
    // Unlock the mutex after completing the update to the shared variable
    pthread_mutex_unlock(&mutex);
    // ----- CRITICAL SECTION ENDS -----
    
    // Exit the thread
    pthread_exit(NULL);
}

int main() {
    // Initialize random number generator with seed based on current time
    srand(time(NULL));
    
    // Initialize the array with random numbers from 0 to 99
    printf("Initializing array with 1 million random numbers...\n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 100;
    }
    
    pthread_t threads[NUM_THREADS];  // Array to store thread IDs
    ThreadData thread_data[NUM_THREADS];  // Array to store data passed to each thread
    
    // Calculate the size of each array portion to be assigned to each thread
    int chunk_size = ARRAY_SIZE / NUM_THREADS;
    
    // Create and launch the threads
    printf("Creating %d threads to calculate sum...\n", NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        // Set up the information for this thread
        thread_data[i].start = i * chunk_size;
        
        // Ensure the last thread processes all remaining elements
        // (in case ARRAY_SIZE is not divisible by NUM_THREADS)
        if (i == NUM_THREADS - 1) {
            thread_data[i].end = ARRAY_SIZE;
        } else {
            thread_data[i].end = (i + 1) * chunk_size;
        }
        
        // Create a new thread and pass the corresponding data
        pthread_create(&threads[i], NULL, sum_array_portion, &thread_data[i]);
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Sum of the array: %lld\n", total_sum);
    
    // Destroy the mutex when no longer needed
    pthread_mutex_destroy(&mutex);
    
    return 0;
}