#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

/* Constant for connection management */
#define MAX_CLIENT  100  // Maximum number of clients that can be connected
#define MAX_SERVER  15   // Maximum number ogf connection request to queue
#define IP_LEN      50   // Maximum length of IP address

/**
 * Define device structure to store connection information 
 * This structure holds ll necessary information about connection
*/
typedef struct
{
    int fd;                     // Socket file descriptor
    int id;                     // Device ID (unique identifier for earch device)
    int port_num;               // Port number of the device
    char my_ip[IP_LEN];         // IP address of the device
    struct sockaddr_in addr;    // Socket address structure
    pthread_t recv_thread;      // Thread id for receiving messages
    bool is_active;             // Flag to indicatie if connection is active
} device;

/** 
 * Function declarations for handling socket connections and device management
*/
int initialize_socket(device *dev, int port);                           // Initialize a socket for the device and bind it
int connect_to_device(device *dev, const char *ip, int port, int id);   // Connect to another device
int disconnect_device(device *dev);                                     // Disconnect from a device and close the socket
void print_device_list();                                               // Print the list of connected devices
void disconnect_all_devices();                                          // Disconnect all devices by closing their sockets

/** 
 * Thread handler for accepting connections 
*/
void *accept_connection_handler(void *args);

#endif