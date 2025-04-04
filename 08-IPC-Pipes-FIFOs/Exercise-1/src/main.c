#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int fd[2];
    pid_t child_pid;

    char *message = "Hello from parrent message";
    char buffer[100];

    // Create pipe
    if (-1 == pipe(fd)) {
        perror("Error creating pipe\n");
        exit(EXIT_FAILURE);
    }

    // Fork a new process
    child_pid = fork();

    if (child_pid < 0) { // Error handling for fork
        perror("Fork failed\n");
        exit(EXIT_FAILURE);
    }
    // Parent process
    else if (child_pid > 0) {
        printf("Parent process (PID: %d) sending message: %s\n", getpid(), message);

        // close read end as parent only writes
        close(fd[0]);

        // Write message to the pipe
        write(fd[1], message, strlen(message) + 1);
        
        // Close write and end after writing is complete
        close(fd[1]);

        // Wait for child to finish
        wait(NULL);
        printf("Paren process terminated\n");
    }
    // Child process
    else {
        // Close write end as only reads
        close(fd[1]);

        // Read data from the pipe
        ssize_t byte_read = read(fd[0], buffer, sizeof(buffer));

        if (byte_read > 0) {
            printf("Child process (PID: %d) received message: %s\n", getpid(), buffer);
        }
        else {
            printf("Child process couldn't read data\n");
        }

        // Close read end after reading is complete
        close(fd[0]);

        printf("Child process terminated\n");
    }

    return 0;
}
