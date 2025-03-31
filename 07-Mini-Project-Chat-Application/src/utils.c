#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

extern pthread_t accept_thread_id;

// Print error message
void print_error(const char *message){
    fprintf(stderr, "ERROR %s\n", message);
}

// Display port number
void print_my_port(int port) {
    printf("My port is :%d\n", port);
}

// Get and display IP address
void print_my_ip(char *ip_buffer) {
    // Open pipe to get IP address via command "hostname -I"
    FILE *fd = popen("hostname -I", "r");

    if (fd == NULL) {
        print_error("Can not get IP address");
        return;
    }

    // Read buffer from pipe
    if (fgets(ip_buffer, 100, fd) == NULL) {
        print_error("Can not get IP address");
        return;
    }

    printf("My IP is : %s", ip_buffer);

    // Close pipe
    pclose(fd);
}

// Handle SIGINT signal
void signal_handler() {
    printf("***********************************************************\n");
    printf("-----------------Program terminated!-----------------------\n");
    printf("***********************************************************\n");
    
    pthread_cancel(accept_thread_id);
    pthread_join(accept_thread_id, NULL);

    exit(EXIT_SUCCESS);
}
