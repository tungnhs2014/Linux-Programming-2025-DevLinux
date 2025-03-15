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
        printf("Child process (Zombie) started: PID = %d\n", getpid());
        
        exit(0);
    }
    else {
        printf("Parent process (Zombie creator) PID = %d\n", getpid());

        // The parent process enters an infinite loop.
        // Because it never calls wait() to collect the child's exit status,
        // the child process remains in the process table as a zombie.
        while(1);
       
    }

    return 0;
}
