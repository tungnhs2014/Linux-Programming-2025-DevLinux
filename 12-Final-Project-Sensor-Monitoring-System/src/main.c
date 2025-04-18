/**
 * @file main.c
 * @brief Main program for the Sensor Monitoring System
 */

#include "config.h"
#include "sbuffer.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

/**
 * Print usage information
 */

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s <port>\n", program_name);
    fprintf(stderr, "<port>: Port name to listen for incoming connection\n");
}

int main(int argc, char const *argv[])
{
    // Check command lne agruments
    if (argc != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Parse port number
    int port = atoi(argv[1]);
    if (port < 0 || port > 65535) {
        fprintf(stderr, "Error: Invalid port number");
        return EXIT_FAILURE;
    }

    // Set up singal handler
    
    // Create shared buffer 
    sbuffer_t *buffer = sbuffer_init();
    if (buffer == NULL) {
        sbuffer_free(&buffer);
        return EXIT_FAILURE;
    }

    // Free shared buffer
    sbuffer_free(&buffer);

    // Start log process
    pid_t log_pid = log_start(LOG_FIFO_PATH, LOG_FILE_PATH);
    if (log_pid < 0) {
        fprintf(stderr, "ErrorL Failed to start log process");
        sbuffer_free(&buffer);
        return EXIT_FAILURE;
    }

    // Open log FIFO for writing
    int log_fd = log_open_fifo(LOG_FIFO_PATH);
    // if (log_fd < 0) {
    //     fprintf(stderr, "Error: Failed to open log FIFO for writing\n");
    //     sbuffer_free(&buffer);
    //     // kill log process
    //     kill(log_fd, SIGTERM);
    //     return EXIT_FAILURE;
    // }
    
    return EXIT_SUCCESS;
}
