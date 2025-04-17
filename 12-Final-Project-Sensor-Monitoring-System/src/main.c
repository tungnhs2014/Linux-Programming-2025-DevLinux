/**
 * @file main.c
 * @brief Main program for the Sensor Monitoring System
 */

#include "config.h"
#include "sbuffer.h"
#include <stdio.h>
#include <stdlib.h>

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

    // Open log FIFO for writing
    int log_fd;

    
    return EXIT_SUCCESS;
}
