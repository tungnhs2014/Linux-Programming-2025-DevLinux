#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Global variable to count the number of SIGINT signals received
int sigint_count = 0;

// Function prototype for the SIGINT signal handler
void sigint_handler(int sig);

int main(int argc, char const *argv[])
{   
    // Register the SIGINT signal handler using signal()
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Failed to register singal handler");
        exit(EXIT_FAILURE);
    }

    printf("Program started. Press Ctrl+C to send SIGINT signal.\n");
    printf("The program will exit after receiving SIGINT 3 times.\n");

    // Infinite loop to keep the program running
    while (1)
    {
        sleep(1);
    }
    
    return 0;
}

void sigint_handler(int sig) {
    sigint_count++;
    printf("\nSIGINT received: %d\n", sigint_count);
    
    // If SIGINT has been received 3 times, exit the program
    if (sigint_count >= 3) {
        printf("Received SIGINT 3 times. Exiting program...\n");
        exit(EXIT_SUCCESS);
    }
}