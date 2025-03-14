#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 

int main(int argc, char const *argv[])
{
    pid_t pid; // Define a variable to store process ID

    // Check if the number of command-line arguments is exactly 2
    if (2 != argc) {
        fprintf(stderr, "Usage: %s <option>\n", argv[0]);
        fprintf(stderr, "Please provide exactly 1 argument\n");
        return -1;
    }
    // Create a child process using fork()
    pid = fork();

    if (0 == pid) {
        printf("Child process running:\n");
        
        // Check the argument passed to the program and execute the appropriate command
        if (strcmp(argv[1], "1") == 0) {
            printf("Executing 'ls' command:\n");
            // Use execlp to replace the child process with the "ls -l" command
            execlp("ls", "ls", "-l", NULL);
        }
        else if (strcmp(argv[1], "2") == 0) {
            printf("Executing 'date' command:\n");
            // Use execlp to replace the child process with the "date" command
            execlp("date", "date", NULL);
        }
        else {
            fprintf(stderr, "Invalid command number: %s, choose (1 or 2)\n", argv[1]);
            exit(1);
        }

        fprintf(stderr, "Execlp failed\n");
        exit(1);
    }
    else if(0 < pid){
        printf("Parrent process running:\n");
        wait(NULL); // Wait for the child process finish
        printf("Child process completed\n");

    }
    else{
        fprintf(stderr, "Fork failed\n");
        return -1;
    }

    return 0;
}

