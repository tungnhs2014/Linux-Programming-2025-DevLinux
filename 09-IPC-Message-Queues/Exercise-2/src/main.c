#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/wait.h>

#define QUEUE_PARENT_TO_CHILD   "/parent_to_child"
#define QUEUE_CHILD_TO_PARENT   "/child_to_parent"
#define MAX_MSG_SIZE            256
#define MSG_PRIO                1

int main(int argc, char const *argv[])
{
    mqd_t mq_parent_to_child, mq_child_to_parent;
    struct mq_attr attr;
    pid_t pid;
    char buffer[MAX_MSG_SIZE];
    int status, count = 0;

    // Set message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create the first messsage queue (parent to child)
    mq_parent_to_child = mq_open(QUEUE_PARENT_TO_CHILD, O_CREAT | O_EXCL | O_RDWR, 0664, &attr);
    if ((mqd_t)(-1) == mq_parent_to_child) {
        // If queue already exists, try opening without O_EXCL
        mq_parent_to_child = mq_open(QUEUE_PARENT_TO_CHILD, O_RDWR);
        if ((mqd_t)(-1) ==  mq_parent_to_child) {
            perror("[parent to child] mq_open failed\n");
            exit(EXIT_FAILURE);
        }
    }

    // Create the second messsage queue (child to parent)
    mq_child_to_parent = mq_open(QUEUE_CHILD_TO_PARENT, O_CREAT | O_EXCL | O_RDWR, 0664, &attr);
    if ((mqd_t)(-1) == mq_child_to_parent) {
        // If queue already exists, try opening without O_EXCL
        mq_child_to_parent = mq_open(QUEUE_CHILD_TO_PARENT, O_RDWR);
        if ((mqd_t)(-1) ==  mq_child_to_parent) {
            perror("[child to parent] mq_open failed\n");
            exit(EXIT_FAILURE);
        }
    }

    // Create child process
    pid = fork();

    if (pid < 0) {
        // Fork failed - clean up and exit
        perror("Fork failed\n");
        mq_close(mq_parent_to_child);
        mq_close(mq_child_to_parent);
        mq_unlink(QUEUE_PARENT_TO_CHILD);
        mq_unlink(QUEUE_PARENT_TO_CHILD);
        exit(EXIT_FAILURE);
    }
    // Child process
    else if (0 == pid) {
        printf("[Child] Process ID: %d\n", getpid());
    } 

    return 0;
}
