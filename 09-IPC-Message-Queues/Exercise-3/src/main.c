#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <ctype.h>

#define QUEUE_NAME "/share_queue" 
#define MAX_MSG_SIZE 256 
#define MSG_PRIORITY 1  

int main() {
    mqd_t mq;
    pid_t pid1, pid2;
    char msg[MAX_MSG_SIZE];

    // Set up the message queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;  // Maximum number of messages
    attr.mq_msgsize = MAX_MSG_SIZE;  // Maximum size of each message
    attr.mq_curmsgs = 0;

    // Create the message queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_EXCL | O_RDWR, 0644, &attr);
    if ((mqd_t)(-1) == mq) {
        // If queue already exists, open it without O_EXCL
        mq = mq_open(QUEUE_NAME, O_RDWR);
        if (mq == (mqd_t)-1) {
            perror("mq_open");
            exit(EXIT_FAILURE);
        }
    }

    printf("[Parent] Process ID: %d\n", getpid());

    // Create the first child process (child 1)
    pid1 = fork();

    if (pid1 < 0) {
        // If fork fails for child 1, print error and exit
        perror("fork (child 1)");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        exit(EXIT_FAILURE);
    } 
    
    else if (pid1 == 0) {
        // Child 1: Receive message, convert to uppercase, and send to child 2
        printf("[Child 1] Process ID: %d\n", getpid());

        mq_close(mq);
        mq = mq_open(QUEUE_NAME, O_RDWR);
        if ((mqd_t)(-1) == mq) {
            perror("mq_open");
            exit(EXIT_FAILURE);
        }

        // Receive the original message from the parent
        if (-1 == mq_receive(mq, msg, MAX_MSG_SIZE, NULL)) {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }

        // Convert the message to uppercase
        for (int i = 0; msg[i]; i++) {
            msg[i] = toupper(msg[i]);
        }

        // Send the uppercase message to child 2
        sleep(1);  // Ensure child 2 is ready before sending
        if (-1 == mq_send(mq, msg, strlen(msg) + 1, MSG_PRIORITY)) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }

        mq_close(mq);
        exit(EXIT_SUCCESS);
    } 
    
    else {
        // Create the second child process (child 2)
        pid2 = fork();

        if (pid2 < 0) {
            // If fork fails for child 2, print error and exit
            perror("fork (child 2)");
            mq_close(mq);
            mq_unlink(QUEUE_NAME);
            waitpid(pid1, NULL, 0);  // Wait for child 1 before exiting
            exit(EXIT_FAILURE);
        }

        else if (pid2 == 0) {
            // Child 2: Receive the uppercase message and print it
            printf("[Child 2] Process ID: %d\n", getpid());

            mq_close(mq);
            mq = mq_open(QUEUE_NAME, O_RDONLY);
            if ((mqd_t)(-1) == mq) {
                perror("mq_open");
                exit(EXIT_FAILURE);
            }

            // Wait for the uppercase message from child 1
            if (mq_receive(mq, msg, MAX_MSG_SIZE, NULL) == -1) {
                perror("mq_receive");
                exit(EXIT_FAILURE);
            }

            // Print the received uppercase message
            printf("Child 2 received message: %s\n", msg);

            mq_close(mq);
            exit(EXIT_SUCCESS);
        }

        else {
            // Parent process: Prepare and send the original message
            const char *original_message = "Hello Tungnhs Nguyen huu son TUng!";
            strncpy(msg, original_message, MAX_MSG_SIZE - 1);
            msg[MAX_MSG_SIZE - 1] = '\0';  // Ensure null termination

            printf("[Parent] Sending message from PID: %d\n", getpid());

            sleep(1);  // sleep to message ready before sending

            // Send the original message to child 1
            if (-1 == mq_send(mq, msg, strlen(msg) + 1, MSG_PRIORITY)) {
                perror("mq_send");
                mq_close(mq);
                mq_unlink(QUEUE_NAME);

                // Wait for children to complete before exiting
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
                exit(EXIT_FAILURE);
            }

            // Wait for both children to finish
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);

            // Clean up the message queue
            mq_close(mq);
            mq_unlink(QUEUE_NAME);

            printf("[Parent] Done.\n");
            exit(EXIT_SUCCESS);
        }
    }
}
