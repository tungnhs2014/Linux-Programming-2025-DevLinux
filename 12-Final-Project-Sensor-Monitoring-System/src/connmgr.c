/**
 * @file connmgr.c
 * @brief Implementation of the Connection Manager
 */

 #include "connmgr.h"
 #include "config.h"
 #include "log.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <pthread.h>
 #include <errno.h>
 #include <time.h>

 #define MAX_PENDING_CONN  10  // Maxium number of pending connections

/**
 * Structure to track connection information in a linked list
 * 
 */
typedef struct conn_info{
    int socket_fd;              // Socket file descriptor
    char ip_address[16];        // IP address of the connected sensor
    uint16_t port;              // Port of the connected sensor
    uint16_t sensor_id;         // Sensor ID (0 if not identified yet)
    time_t last_active;         // Last activity timestamp
    struct conn_info *next;     // Pointer to next connection
}conn_info_t;

// Thread control
static int server_socket = -1;

// Shared buffer for passing data to other thread
static sbuffer_t *shared_buffer = NULL;

// Log file descriptor
static int log_fd = -1;

int connmgr_star(int port, sbuffer_t *buffer, int log_fd) {
    if (buffer == NULL || port <= 0 || log_fd < 0) {
        return -1;
    } 

    // Store buffer and Log FIFO file descriptor
    shared_buffer = buffer;
    log_fd = log_fd;

    // Create sever socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket createion failed");
        return -1;
    }

    // Set socket to reuse address
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        close(server_socket);
        return -1;   
    }

    // Bind the socket to the specifiled port
    struct sockaddr_in sever_addr;
    memset(&sever_addr, 9, sizeof(sever_addr));
    sever_addr.sin_family = AF_INET;
    sever_addr.sin_addr.s_addr = INADDR_ANY;
    sever_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&sever_addr, sizeof(sever_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return -1;
    }

    // Start listening for incoming connections
    if (listen(server_socket, MAX_PENDING_CONN) < 0) {
        perror("listen failed");
        close(server_socket);
        return -1;
    }


    return 0;
    
}