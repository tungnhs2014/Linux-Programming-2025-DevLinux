#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/wait.h>

#define QUEUE_NAME      "/mqueue"
#define MAX_MSG_SIZE    256
#define MSG_PRIO        1

int main(int argc, char const *argv[])
{
    mqd_t mq;
    struct mq_attr attr;
    pid_t pid;
    char buffer[MAX_MSG_SIZE];
    int status;

    // Set message queue attributes
    attr.mq_curmsgs = 0;             // Behavior flags (0: blocking mode)
    attr.mq_maxmsg = 10;             // Maximum number of message in queue
    attr.mq_msgsize = MAX_MSG_SIZE;  // Maxium size of a message in bytes
    attr.mq_curmsgs = 0;             // Current number of message in queue

    // Create the message with read/write access
    mq = mq_open(QUEUE_NAME, O_CREAT | O_EXCL | O_RDWR, 0644, &attr);   
    if (-1 == mq) {
        // If queue exists, try to opne it without the O_EXCL flag
        mq = mq_open(QUEUE_NAME, O_RDWR);
        if (-1 == mq) {
            perror("mq_open failed\n");
            exit(EXIT_FAILURE);
        }
    }

    // Create a child process using fork()
    pid = fork();

    if (pid < 0) {
        // Fork failed - clean up and exit
        perror("fork failed\n");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        exit(EXIT_FAILURE);
    }
    // Child process
    else if (pid == 0) {
        printf("Child process ID: %d\n", getpid());

        // Close the message queue descriptor
        mq_close(mq);

        // Open the queue for reading 
        mq = mq_open(QUEUE_NAME, O_RDONLY);
        if (-1 == mq) {
            perror("[Child] mq_open_faield");
            exit(EXIT_FAILURE);
        }

        // Initialize buffer to zeros to ensure proper string termination
        memset(buffer, 0, MAX_MSG_SIZE);

        /* Receive message 
            - buffer: Where to store the message
            - MAX_MSG_SIZE: Maximum number of bytes to receive
            - NULL: Don't store priority value
        */
        if(-1 == mq_receive(mq, buffer, MAX_MSG_SIZE, NULL)) {
            perror("[Child] mq_receive failed\n");
            mq_close(mq);
            exit (EXIT_FAILURE);
        }
        
        // Print the received message
        printf("[Child] Message received: %s\n", buffer);
        
        // Close the queue in the child process
        mq_close(mq);
        exit(EXIT_SUCCESS);
    }
    // Parent process
    else {
        printf("Parent process ID: %d\n", getpid());

        // Message to send to child process
        char *message = "Hello TungNHS";

        // Small delay to ensure child process is ready to receive
        sleep(1);

        /* Send message
            - message: The message to send
            - strlen(message) + 1: Length of message (including null terminator)
            - MSG_PRIO: Priority of this message
        */
        if(-1 == mq_send(mq, message, strlen(message) + 1, MSG_PRIO)) {
            perror("[Parent] mq_send failed\n");
            mq_close(mq);
            mq_unlink(QUEUE_NAME);
            exit(EXIT_FAILURE);
        }

        printf("[Parent] Sending message: %s\n", message);

        // Wait for child process to complete
        waitpid(pid, &status, 0);
        
        // Clean up by closing and removing the message queue
        printf("[Parent] Cleaning up message queue...\n");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);

        exit(EXIT_SUCCESS);
    }

    return 0;
}
