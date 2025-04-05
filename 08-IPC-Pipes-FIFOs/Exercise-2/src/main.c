#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int pipe1[2]; // Pipe for parent to child1 communication
    int pipe2[2]; // Pipe for child1 to child2 communication
    pid_t child1_pid, child2_pid;

    char *message = "Hello TungNHS";
    char buffer1[100], buffer2[200];

    // Create the first pipe (parent => child1)
    if (-1 == pipe(pipe1)) {
        perror("Error creating first pipe\n");
        exit(EXIT_FAILURE);
    }
    
    // Create the second pipe (child1 => child2)
    if (-1 == pipe(pipe2)) {
        perror("Error creating second pipe\n");
        exit(EXIT_FAILURE);
    }

    // Fork first child process
    child1_pid = fork();
    
    if (child1_pid < 0) {
        perror("Error on fist fork\n");
        exit(EXIT_FAILURE);
    }
    // Parent process
    else if (child1_pid > 0) {
        // Fork second child process
        child2_pid = fork();

        if (child2_pid < 0) {
            perror("Error on second fork\n");
            exit(EXIT_FAILURE);
        }

        else if (child2_pid > 0) { // Stil in parent process
            printf("Parent process (PID: %d) sending a message: %s\n", getpid(), message);

            // Close unused pipe ends in parent
            close(pipe1[0]); // Close read end of pipe1
            close(pipe2[0]); // Close read end of pipe2
            close(pipe2[1]); // Close write end of pipe2

            // Write message to first child through pipe1
            write(pipe1[1], message, strlen(message) + 1); // +1 is used to include null characters ('\0')
            printf("Parent sent message to Child 1\n");

            // Close write and end of pipe1 after using
            close(pipe1[1]);
            
            // Wait for both children to finish 
            wait(NULL);
            wait(NULL);

            printf("Parent process terminated\n");
        }
        // Child 2 process
        else {
            // Close unused pipe ends in child2
            close(pipe1[0]); // Close read of end pipe1
            close(pipe1[1]); // Close write of end pipe1
            close(pipe2[1]); // Close write of end pipe2

            // Read data from Child 1 to Child 2 through pipe2
            ssize_t read_byte = read(pipe2[0], buffer2, sizeof(buffer2));
            if (read_byte > 0) {
                printf("Child 2 process (PID: %d) received message: %s\n", getpid(), buffer2);
            }
            else {
                printf("Child 2 coudn't read data\n");
            }
            
            // Close read end of pipe2 after use
            close(pipe2[0]);

            printf("Child 2 process terminated\n");
            exit(EXIT_SUCCESS);
        }
    }
    // Child 1 process
    else {  
        // Closed unused pipe end in Child 1
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2

        // Read data from parent to Child 1 through pipe1
        ssize_t read_byte = read(pipe1[0], buffer1, sizeof(buffer1));
        if (read_byte > 0) {
            printf("Child 1 process (PID: %d) received message: %s\n", getpid(), buffer1);

            // Modify the message
            snprintf(buffer2, sizeof(buffer2), "%s - Nguyen Huu Son Tung", buffer1);
            printf("Child 1 modified message: %s\n", buffer2);

            // Send modified message to Child 2 through pipe2
            write(pipe2[1], buffer2, strlen(buffer2) + 1);
            printf("Child 1 sent message to Child 2\n");
        }
        else {
            printf("Child 1 coudn't read data\n");
        }  

         // Close used pipe ends after use
         close(pipe2[1]);
         close(pipe1[0]);

        printf("Child 1 process terminated\n");
        exit(EXIT_SUCCESS);
    }

    return 0;
}
