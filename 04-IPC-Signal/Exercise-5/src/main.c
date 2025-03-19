#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>

// Prototype singal handler function
void sigint_handler(int sig);
void sigterm_handler(int sig);

int main(int argc, char const *argv[]) {
    // Register the signal handler for SIGINT
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Failed to register SIGINT handler\n");
        exit(EXIT_FAILURE);
    }
    // Register the signal handler for SIGTERM (kill command or termination signal)
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
        perror("Failed to register SIGTERM handler");
        exit(EXIT_FAILURE);
    }
    
    printf("Program started. Press Ctrl + C to send SIGINT, or send SIGTERM to terminate \n");

    char input[256]; // Buffer to store user input
    fd_set readfds; // File descriptor set for select()
    int retval; // Return value from select()

    while (1) {
        // Clear the file descriptor set
        FD_ZERO(&readfds);

        // Add the standard input (keyboard) to the file descriptor set
        FD_SET(STDIN_FILENO, &readfds);

        // Call select() to wait for input or signal
        retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);
        
        // Check if select() an error
        if (retval == -1) {
            perror("select() failed\n");
            exit(EXIT_FAILURE);
        }

        // Check if there is input available on standard input (stdin)
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            // Read data from stdin when available
            if (fgets(input, sizeof(input), stdin) != NULL) {
                printf("User input: %s", input);
            }
        }

    }

    return 0;
}

// Signal handler function for SIGINT (Ctrl + C)
void sigint_handler(int sig) {
    printf("\nSIGINT received\n");
}

// Signal handler function for SIGTERM (Termination signal)
void sigterm_handler(int sig) {
    printf("\nSIGTERM received. Exiting program...\n");
    exit(EXIT_SUCCESS);
}

