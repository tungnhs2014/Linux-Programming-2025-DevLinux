#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Prototype signal handler function
void sigtstp_handler(int sig);

int main(int argc, char const *argv[]) {
    // Register the signal handler for SIGTSTP using the signal() function
    if (signal(SIGTSTP, sigtstp_handler) == SIG_ERR) {
        perror("Failed to register signal handler\n");
        exit(EXIT_FAILURE);
    }

    printf("Program started. Press Ctrl + Z to test SIGTSTP handling\n");

    int count = 0;  // Counter to keep track of iterations

    while (1) {
        printf("Programing is running count: %d\r", count++);
        fflush(stdout);  // Force output to be displayed immediately
        sleep(1);
    }
    return 0;
}

// Signal handler function to handle SIGTSTP (Ctrl + Z)
void sigtstp_handler(int sig) {
    printf("\nSIGTSTP ignored\n");
    fflush(stdout); // Ensure the message is displayed immediately
}