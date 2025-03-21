#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Thread function prototype
void* thread_function(void* arg);

int main(int argc, char const *argv[])
{
    // Declare an array of thread IDs
    pthread_t threads[2];
    int thread_ids[2];
    int ret;

    // Create two threads
    for (int i = 0; i < 2; i++) {
        // Define a unique ID to each thread
        thread_ids[i] = i + 1;

        // Create a new thread and pass the ID as an argument
        ret = pthread_create(&threads[i], NULL, thread_function, (void*)&thread_ids[i]);
        // Check if thread creation was successful
        if (ret) {
            printf("Error creating thread %d: %d\n", i + 1, ret);
            exit(EXIT_FAILURE);
        }

        printf("Main: Created thread %d\n", i + 1);   
    }

    // Wait for all threads to complete
    for (int i = 0; i < 2; i++) {
        ret = pthread_join(threads[i], NULL);

        // Check if joining the thread
        if(ret) {
            printf("Error joining thread %d: %d\n", i + 1, ret);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

// Thread function to print a message
void* thread_function(void* arg) {
    // Get the thread ID from the argument
    int thread_id = *((int*)arg);

    printf("Thread %d: Hello from thread!\n", thread_id);
    
    // Exit the thread
    pthread_exit(NULL);
}