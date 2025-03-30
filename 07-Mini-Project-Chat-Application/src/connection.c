#include "connection.h"
#include "utils.h"
#include <pthread.h>

extern device this_device;
extern device device_connect_to[MAX_CLIENT];
extern device device_connect_from[MAX_CLIENT];

extern int total_device_to;                   
extern int total_device_from;

// Initialize socket for the current device
int initialize_socket(device *dev, int port) {
    // Create socket for current device
    dev->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == dev->fd) {
        print_error("Socket createion failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(dev->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        print_error("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Set address and port for current device
    dev->port_num = port;
    dev->addr.sin_family = AF_INET;
    dev->addr.sin_addr.s_addr = INADDR_ANY;
    dev->addr.sin_port = htons(dev->port_num);
    
    // Blind the socket to the device's IP address and port
    if (-1 == bind(dev->fd, (struct sockaddr*)&dev->addr, sizeof(dev->addr))) {
        print_error("Blind failed");
        exit(EXIT_FAILURE);
    }

    // Put the device in listening mode for connection
    if (-1 == listen(dev->fd, MAX_SERVER)) {
        print_error("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Listening on port: %d\n", dev->port_num);
    return 0;
}

// Thread function to accept connections

void *accept_connection_handler(void *args) {
    int client_fd;
    struct sockaddr_in client_addr;
    
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        client_fd = accept(this_device.fd, (struct sockaddr *)&client_addr, &client_len);
        if (-1 == client_fd) {
            print_error("Accept new device failed");
            return NULL;
        }
    }

    // Save newly connected device information
    device_connect_from[total_device_from].fd = client_fd;
    device_connect_from[total_device_from].id = total_device_from;
    device_connect_from[total_device_from].addr = client_addr;
    device_connect_from[total_device_from].port_num = ntohs(client_addr.sin_port);
    // inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, device_connect_from[total_device_from].my_ip, IP_LEN);

}