#include "utils.h"
#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <net/if.h>
#include <signal.h>

/* External variables */
extern device this_device;
extern pthread_t accept_thread_id;
extern pthread_mutex_t device_list_mutex;

/**
 * Print an error message to stderr
 * 
 * @param message Error message to print
 */
void print_error(const char *message) {
    if (!message) return;
    fprintf(stderr, "ERROR: %s\n", message);
}

/**
 * Display the port number of the device
 * 
 * @param port Port number to display
 */
void print_my_port(int port) {
    printf("My port is: %d\n", port);
}

/**
 * Get the IP address of the current device
 * Uses a simpler approach to get the primary IP address
 * 
 * @param ip_buffer Buffer to store the IP address
 * @param buffer_len Length of the buffer
 * @return true if IP was found, false otherwise
 */
bool get_my_ip(char *ip_buffer, size_t buffer_len) {
    // Validate input
    if (!ip_buffer || buffer_len < 16) return false;
    
    // Use system command to get IP - simpler approach
    FILE *fd = popen("hostname -I | awk '{print $1}'", "r");
    
    if (!fd) {
        print_error("Failed to execute hostname command");
        return false;
    }
    
    // Read output from command
    if (fgets(ip_buffer, buffer_len, fd) != NULL) {
        // Remove newline if present
        size_t len = strlen(ip_buffer);
        if (len > 0 && ip_buffer[len-1] == '\n') {
            ip_buffer[len-1] = '\0';
        }
        
        pclose(fd);
        return true;
    }
    
    pclose(fd);
    
    // Fallback to localhost if no IP found
    strncpy(ip_buffer, "127.0.0.1", buffer_len - 1);
    ip_buffer[buffer_len - 1] = '\0';
    
    return true;
}

/**
 * Display the IP address
 * 
 * @param ip_buffer Buffer containing the IP address
 */
void print_my_ip(char *ip_buffer) {
    if (!ip_buffer) return;
    
    printf("My IP is: %s\n", ip_buffer);
}

/**
 * Clear input buffer to avoid issues with fgets
 */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Discard characters until newline or EOF
    }
}

/**
 * Validate if a string is a valid IP address
 * 
 * @param ip String containing IP address to validate
 * @return true if valid, false otherwise
 */
bool is_valid_ip(const char *ip) {
    if (!ip) return false;
    
    // Use inet_pton to validate the IP format
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
}

/**
 * Clean up all resources before exiting
 * Closes sockets, terminates threads, and destroys mutexes
 */
void cleanup_resources() {
    printf("Cleaning up resources...\n");
    
    // Disconnect all devices
    disconnect_all_devices();
    
    // Cancel and join the accept thread
    if (accept_thread_id) {
        pthread_cancel(accept_thread_id);
        pthread_join(accept_thread_id, NULL);
    }
    
    // Close the main socket
    if (this_device.fd > 0) {
        close(this_device.fd);
        this_device.fd = -1;
    }
    
    // Destroy mutex
    pthread_mutex_destroy(&device_list_mutex);
    
    printf("All resources cleaned up.\n");
}

/**
 * Signal handler to gracefully shut down the application
 * Handles SIGINT (Ctrl+C) to ensure clean program termination
 * 
 * @param sig Signal number
 */
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n***********************************************************\n");
        printf("-----------------Program terminated!-----------------------\n");
        printf("***********************************************************\n");
        
        // Clean up all resources before exiting
        cleanup_resources();
        exit(EXIT_SUCCESS);
    }
}