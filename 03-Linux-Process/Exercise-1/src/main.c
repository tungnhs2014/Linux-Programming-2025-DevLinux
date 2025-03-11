#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h> 

int main(int argc, char const *argv[])
{
    pid_t pid; // Define a variable to store process ID

    // Create a child process using fork()
    pid = fork();

    if(0 == pid){
        printf("Child process:\n");
        printf("PID of child process: %d\n", getpid());
        printf("PID of parent process: %d\n", getppid());

        sleep(5); // sleep for 5 seconds to simulate child process running
    }
    else if(0 < pid){
        printf("Parrent process:\n");
        printf("PID of parent process: %d\n", getpid());

        wait(NULL); // Wait for the child process finish
    }
    else{
        fprintf(stderr, "Fork failed\n");
        return -1;
    }

    return 0;
}

