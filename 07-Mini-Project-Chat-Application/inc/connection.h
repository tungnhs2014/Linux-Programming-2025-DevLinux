#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENT 100
#define MAX_SERVER 15
#define IP_LEN     50

// Define device structure to store connection information
typedef struct
{
    int fd;                     // Socket file descriptor
    int id;                     // Device ID
    int port_num;               // Port number
    char my_ip[IP_LEN];         // IP address
    struct sockaddr_in addr;    // Socket address structure
} device;


// Connection management functions
int initialize_socket(device *dev, int port);
int connect_to_device(device *dev, const char *ip, int port, int id);
int disconnect_device(device *dev);
void print_device_list(device *device_list, int total_devices);
void disconnect_all_device(device *device_list, int total_decvices);

// Thread handler for accepting connections
void *accept_connection_handler(void *args);
#endif