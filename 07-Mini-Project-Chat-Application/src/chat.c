#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "connection.h"
#include "utils.h"
#include "command.h"

/**  
 * Global variables to manage device connection
*/
device this_device = {0};                         // Holds the current device's information

/* Arrays to track connection */
device device_connect_to[MAX_CLIENT] = {0};       // List of devices this device is connected to
device device_connect_from[MAX_CLIENT] = {0};     // List of devices that are connected to this device

/* Connection to counters */
int total_device_to = 0;                          // Total number of outgoing connections
int total_device_from = 0;                        // Total number of incoming connections

/* Main thread for accepting connections */
pthread_t accept_thread_id;                       // Thread ID for accepting connection                 

/* Mutex for synchronizing access to shared resources */
pthread_mutex_t device_list_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Main entry point of the program
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return Exit status code
 */
int main(int argc, char const *argv[])
{
    // Check command line arguments - require port number
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize device strutures - set all connection to inactive 
    for (int i = 0; i < MAX_CLIENT; i++) {
        device_connect_to[i].fd = -1;
        device_connect_to[i].is_active = false;
        device_connect_from[i].fd = -1;
        device_connect_from[i].is_active = false;
    }

    // Register signal handler for SIGINT (Ctrl + C)
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        print_error("Cannot handle SIGINT");
        exit(EXIT_FAILURE);
    }

    // Parse and validate port number from command line
    int port = atoi(argv[1]);      // Convert port number from string to integer
    if (port <= 0 || port > 65535) {
        print_error("Invalid port number. Port must be between 1 and 65335");
        exit(EXIT_FAILURE);
    }

    // Initialize socket with the specified port
    if (initialize_socket(&this_device, port)) {
        print_error("Can not initialize socket");
        exit(EXIT_FAILURE);
    }

    // Get and store the device's address
    if (!get_my_ip(this_device.my_ip, IP_LEN)) {
        print_error("Cannot get device IP");
        exit(EXIT_FAILURE);
    }

    // Print the list of available commands
    print_command_list();

    // Create thread for accepting incomming connections
    if (pthread_create(&accept_thread_id, NULL, accept_connection_handler, NULL)) {
        print_error("Can not create thread for accepting new devices");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }
    
    // Main loop to process user command
    char command[256];
    while (1) {
        printf("Enter command: ");
        if(fgets(command, sizeof(command), stdin) == NULL) {
            // Handle EOF or error in input
            break;
        }
        // Process the entered command
        process_command(command);
    }
    
    // Clean up resources and exit
    cleanup_resources();
    
    return 0;
}
