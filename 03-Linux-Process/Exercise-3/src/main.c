#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h> 

void signal_handler(int sig);

int main(int argc, char const *argv[])
{
    pid_t pid; // Define a variable to store process ID

    // Create a child process using fork()
    pid = fork();

    if (0 > pid) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }

    else if (0 == pid) {
        printf("Child process started: PID: %d\n", getpid());

        // Register the signal handler for SIGUSR1
        signal(SIGUSR1, signal_handler);

         // Keep the child process alive to wait for the signal
        sleep(10);
    }
    else {
        printf("Parent process started: PID: %d\n", getpid());
        
        // Sleep for 2 seconds before sending the signal
        sleep(2);

        // Send the SIGUSR1 signal to the child process
        kill(pid, SIGUSR1);
        printf("Parent set SIGUSR1 to child process\n");

        // Wait for the child process to finish
        wait(NULL);

    }

    return 0;
}

// Signal handler function for SIGUSR1
void signal_handler(int sig) {
    printf("Child process received signal SIGUSR1\n");
    exit(1);

}