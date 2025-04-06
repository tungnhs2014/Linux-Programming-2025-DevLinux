#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "connection.h"
#include "utils.h"
#include "command.h"

// Global variables
device this_device;                         // Current device
device device_connect_to[MAX_CLIENT];       // List of device connected to
device device_connect_from[MAX_CLIENT];     // List of device connected from

int total_device_to = 0;                    // Total number of outgoing connections
int total_device_from = 0;                  // Total number of incoming connections

pthread_t accept_thread_id, receive_thread_id;

int main(int argc, char const *argv[])
{
    // Check command line arguments
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Register signal handler for SIGINT (Ctrl + C)
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        print_error("Can not handle SIGINT");
        exit(EXIT_FAILURE);
    }

    // Initialize socket with port from command line
    int port = atoi(argv[1]);
    if (initialize_socket(&this_device, port)) {
        print_error("Can not initialize socket");
        exit(EXIT_FAILURE);
    }

    // Display command list
    print_command_list();

    // Create thread to accept connections
    if (pthread_create(&accept_thread_id, NULL, accept_connection_handler, NULL)) {
        print_error("Can not create thread for accepting new devices");
        exit(EXIT_FAILURE);
    }

    char command[256];
    while (1) {
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);
        process_command(command);
    }

    return 0;
}
