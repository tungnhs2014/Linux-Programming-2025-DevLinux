#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h> 


int main(int argc, char const *argv[])
{
    pid_t pid; // Define a variable to store process ID

    int status;

    // Create a child process using fork()
    pid = fork();

    if (0 > pid) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }

    else if (0 == pid) {
        printf("Child process started: PID: %d\n", getpid());

        // Generate a randome exit code between 0 and 99
        int exit_code = rand() % 100;
        printf("Child process exiting with code: %d\n", exit_code);

        // while(1); Simulate child process did not exit normally by using kill -9

        exit(exit_code); // Exit child process with the generated exit code 

    }
    else {
        // Wait for the child process to finish and retrieve the status
        int ret = wait(&status);

        if (-1 == ret) {
            fprintf(stderr, "Wait unsucessfully\n");
            exit(1);
        }

        printf("Parent process: child process PID: %d\n", ret);

        // Check if the child process exited normally
        if (WIFEXITED(status)) {
            printf("Child process exited normally with status: %d\n", WEXITSTATUS(status));
        } 
        else {
            printf("Child process did not exit normally\n");
        }

    }

    return 0;
}
