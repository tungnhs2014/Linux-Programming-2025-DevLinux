#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int fd[2]; // File descriptor for pipe
    pid_t child_pid;
    char *message = "Welcome Nguyen Huu Son Tung";
    char buffer[256];

    // Create pipe
    if (-1 == pipe(fd)) {
        perror("Error creating pipe\n");
        exit (EXIT_FAILURE);
    }

    // Fork a new process
    child_pid = fork();

    if (child_pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    // Parent process
    else if (child_pid > 0) {
        printf("Parent process (PID: %d) sending a message: %s\n", getpid(), message);

        // Closed unused pipe - read end
        close(fd[0]);

        // Write a message to pipe
        write(fd[1], message, strlen(message) + 1);  // +1 is used to include null characters ('\0')

        // Close write end after using
        close(fd[1]);

        // Wait for child to finish
        wait(NULL);
        printf("Parent process terminated\n");
    }
    // Child process
    else {
        // Closed unused pipe - write end
        close(fd[1]);
        
        // Read data from the pipe
        ssize_t read_data = read(fd[0], buffer, sizeof(buffer));
        if (read_data > 0) {
            int count = strlen(buffer);
            printf("Child process (PID: %d) received a message: %s\n", getpid(), buffer);
            printf("Child process counted %d characters in the received string\n", count);
        }
        else {
            printf("Child process couldn't read data");
        }

        // Close read end after using
        close(fd[0]);
        printf("Child process terminated\n");
        exit(EXIT_SUCCESS);
    }

    return 0;
}
