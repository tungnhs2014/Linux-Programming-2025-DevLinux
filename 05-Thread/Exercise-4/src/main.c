#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ARRAY_SIZE 100

// Global variables to store the counts of even and odd numbers
int event_count = 0;
int odd_count = 0;
int numbers[ARRAY_SIZE];

// Thread function prototype
void* count_even(void *arg);
void* count_odd(void *arg);

int main(int argc, char const *argv[])
{
    pthread_t even_thread, odd_thread;
    int ret;

    // Initialize the random number generator
    srand(time(NULL));

    // Generate 100 randome numbers between 1 and 100
    for (int i = 0; i < ARRAY_SIZE; i++) {
        numbers[i] = rand() % 100 + 1;
    }

    // Create the even counting thread
    ret = pthread_create(&even_thread, NULL, count_even, NULL);
    if (ret) {
        perror("ERROR: Creating even thread\n");
        exit(EXIT_FAILURE);
    }

    // Create the odd counting thread
    ret = pthread_create(&odd_thread, NULL, count_odd, NULL);
    if (ret) {
        perror("ERROR: Creating odd thread\n");
        exit(EXIT_FAILURE);
    }

    // Wait for both threads to finish
    pthread_join(even_thread, NULL);
    pthread_join(odd_thread, NULL);

    // Print the final counts
    printf("Total even numbers: %d\n", event_count);
    printf("Total count numbers: %d\n", odd_count);
    printf("Total numers: %d\n", event_count + odd_count);

    return 0;
}

// Thread function to count even numbers
void* count_even(void *arg) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (numbers[i] % 2 == 0) {
            event_count++;
        }
    }
    pthread_exit(NULL);
}

// Thread function to count old numbers
void* count_odd(void *arg) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (numbers[i] % 2 != 0) {
            odd_count++;
        }
    }
    pthread_exit(NULL);
}
