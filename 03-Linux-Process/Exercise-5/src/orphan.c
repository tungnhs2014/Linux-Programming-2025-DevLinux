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
        // The child process sleeps for 10 seconds, this delay allows parrent process to exit before the child process
        // Making the child an orphan process
        // verify this by running: ps -ef | grep -iE <program_name>
        sleep(10);
        printf("Orphan process running, adopted by init (PID = 1), PID = %d, PPID = %d\n", getpid(), getppid());
        
        exit(0);
    }
    else {
        printf("Parent process (Orphan creator) PID = %d\n", getpid());
        
        exit(0);
    }

    return 0;
}
