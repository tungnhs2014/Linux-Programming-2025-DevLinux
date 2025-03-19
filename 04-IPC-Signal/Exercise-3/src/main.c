#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Global variable to count the number of signals sent
int signal_count = 0;

// Prototype signal handler function
void signal_handler(int sig);

int main(int argc, char const *argv[]) {
    pid_t child_pid;

    // Create a new child process using fork()
    child_pid = fork();

    // Check if fork() failed
    if (0 > child_pid) {
        perror("Fork() failed\n");
        exit(EXIT_FAILURE); 
    }

    // Child process
    else if (0 == child_pid) {
        printf("Child process started. PID: %d\n", getpid());

        // Register a signal handler for SIGINT in the child process
        if (signal(SIGINT, signal_handler) == SIG_ERR) {
            perror("Failed to register signal handler in child\n");
            exit(EXIT_FAILURE);
        }

        // loop to keep the child process running
        while (1) {
            sleep(1);  
        }

        exit(EXIT_SUCCESS); 
    }
    
    // Parent process
    else {
        printf("Parent process. PID: %d, Child PID: %d\n", getpid(), child_pid);

        // Loop to send 5 signals to the child process
        while (signal_count < 5) {
            sleep(2);  // Sleep for 2 seconds before sending the next signal

            // Send SIGUSR1 signal to the child process
            if (kill(child_pid, SIGUSR1) < 0) {
                perror("Failed to send signal\n");
                exit(EXIT_FAILURE);
            }

            // Increment the signal count and print a message
            signal_count++;
            printf("Sent signal %d to child process\n", signal_count);
        }
    
        
        printf("Sent 5 signals. Terminating child process...\n");

        // Send SIGTERM to terminate the child process
        kill(child_pid, SIGTERM);

        // Wait for the child process to terminate
        wait(NULL);

        printf("Child process terminated. Parent exiting\n");
    }

    return 0;
}

// Signal handler function to handle the received signal in the child process
void signal_handler(int sig) {
    printf("Received signal from parent process\n");
    exit(EXIT_SUCCESS); 
}
