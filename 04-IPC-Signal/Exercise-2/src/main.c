#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Global variable to keep track of the timer count
int timer_count = 0;

// Function prototype for the alarm signal handler
void alarm_handler (int sig);

int main(int argc, char const *argv[])
{
    // Register the signal handler for SIGALRM
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        perror("Failed to register singal handler");
        exit(EXIT_FAILURE);
    }

    printf("Timer program started. Will count to 10 seconds.\n");
    
    // Set an alarm to trigger SIGALRM after 1 second
    alarm(1);

    // Main loop to keep the program running
    while (1)
    {
        sleep(1);
    }
    
    return 0;
}

void alarm_handler(int sig) {
    timer_count++;
    printf("\nTime: %d seconds\n", timer_count);

    // Check if the timer count has reached 10 seconds
    if (timer_count >= 10) {
        printf("Timer reached 10 seconds. Exiting program...\n");
        exit(EXIT_SUCCESS);
    }
    
    // Reset the alarm for the next second to keep counting
    alarm(1);
}