#include "connection.h"
#include "message.h"
#include "utils.h"
#include <pthread.h>
#include <errno.h>

/* External global variables */
extern device this_device;
extern device device_connect_to[MAX_CLIENT];
extern device device_connect_from[MAX_CLIENT];
extern int total_device_to;
extern int total_device_from;
extern pthread_mutex_t device_list_mutex;

/**
 * Initialize the socket for the current device
 * Creates a socket, sets options, binds to port, and starts listening
 * 
 * @param dev Pointer to device structure to initialize
 * @param port Port number to bind to
 * @return 0 on success, -1 on failure
 */
int initialize_socket(device *dev, int port) {
    // Validate input
    if (!dev) return -1;
    
    // Create a TCP socket
    dev->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (dev->fd == -1) {
        print_error("Socket creation failed");
        return -1;
    }

    // Set socket options to allow reusing the address
    // This helps avoid "Address already in use" errors when restarting
    int opt = 1;
    if (setsockopt(dev->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        print_error("setsockopt failed");
        close(dev->fd);
        return -1;
    }

    // Configure device address with the specified port
    dev->port_num = port;
    dev->addr.sin_family = AF_INET;
    dev->addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any interface
    dev->addr.sin_port = htons(dev->port_num);  // Convert to network byte order
    dev->is_active = true;

    // Bind the socket to the device address
    if (bind(dev->fd, (struct sockaddr*)&dev->addr, sizeof(dev->addr)) == -1) {
        print_error("Bind failed");
        close(dev->fd);
        return -1;
    }

    // Start listening for incoming connections
    if (listen(dev->fd, MAX_SERVER) == -1) {
        print_error("Listen failed");
        close(dev->fd);
        return -1;
    }

    printf("Listening on port: %d\n", dev->port_num);
    return 0;
}

/**
 * Thread handler for accepting incoming connections
 * Runs in background waiting for new connections
 * 
 * @param args Thread arguments (not used)
 * @return NULL when thread exits
 */
void *accept_connection_handler(void *args) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        // Accept new incoming connection
        int client_fd = accept(this_device.fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            if (errno == EINTR) {
                // Interrupted by signal, just continue
                continue;
            }
            print_error("Accept new device failed");
            continue;
        }

        // Find an available slot and create device info
        int idx = -1;
        device* new_device = NULL;
        
        pthread_mutex_lock(&device_list_mutex);
        
        // Find available slot
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (!device_connect_from[i].is_active) {
                idx = i;
                break;
            }
        }
        
        if (idx == -1) {
            // No space for new connection
            pthread_mutex_unlock(&device_list_mutex);
            close(client_fd);
            print_error("Maximum incoming connections reached");
            continue;
        }

        // Save the connection information
        device_connect_from[idx].fd = client_fd;
        device_connect_from[idx].id = idx;
        device_connect_from[idx].addr = client_addr;
        device_connect_from[idx].port_num = ntohs(client_addr.sin_port);
        device_connect_from[idx].is_active = true;
        
        // Convert IP address to string
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, 
                 device_connect_from[idx].my_ip, IP_LEN);
        
        // Create a copy of device for thread parameter
        new_device = malloc(sizeof(device));
        if (!new_device) {
            device_connect_from[idx].is_active = false;
            pthread_mutex_unlock(&device_list_mutex);
            close(client_fd);
            print_error("Memory allocation failed");
            continue;
        }
        
        // Copy device data and update counter
        memcpy(new_device, &device_connect_from[idx], sizeof(device));
        total_device_from++;
        
        pthread_mutex_unlock(&device_list_mutex);

        // Create a thread to handle receiving messages
        pthread_t recv_thread;
        if (pthread_create(&recv_thread, NULL, receive_message_handler, new_device)) {
            print_error("Cannot create thread to receive message");
            free(new_device);
            
            pthread_mutex_lock(&device_list_mutex);
            device_connect_from[idx].is_active = false;
            total_device_from--;
            pthread_mutex_unlock(&device_list_mutex);
            
            close(client_fd);
            continue;
        }
        
        // Store thread ID and detach it
        device_connect_from[idx].recv_thread = recv_thread;
        pthread_detach(recv_thread);
        
        printf("New connection accepted from %s:%d\n", 
               device_connect_from[idx].my_ip, 
               device_connect_from[idx].port_num);
    }
    
    return NULL;
}

/**
 * Print the list of devices connected
 * Displays all active connections (both outgoing and incoming)
 */
void print_device_list() {
    pthread_mutex_lock(&device_list_mutex);

    printf("\n****************** Device Connections *******************\n");
    printf("ID |        IP Address         | Port No.\n");
    printf("--------------------------------------------------------\n");

    // Print outgoing connections (connections we initiated)
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (device_connect_to[i].is_active && device_connect_to[i].fd > 0) {
            printf("%-3d| %-25s | %-d\n", 
                   device_connect_to[i].id, 
                   device_connect_to[i].my_ip, 
                   device_connect_to[i].port_num);
        }
    }

    // Print incoming connections (connections others initiated to us)
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (device_connect_from[i].is_active && device_connect_from[i].fd > 0) {
            printf("%-3d| %-25s | %-d (incoming)\n", 
                   device_connect_from[i].id + MAX_CLIENT, 
                   device_connect_from[i].my_ip, 
                   device_connect_from[i].port_num);
        }
    }
    
    printf("********************************************************\n\n");

    pthread_mutex_unlock(&device_list_mutex);
}

/**
 * Connect to another device
 * Establishes a TCP connection to a remote peer
 * 
 * @param dev Device structure to store connection info
 * @param ip IP address to connect to
 * @param port Port to connect to
 * @param id ID to assign to this connection
 * @return 0 on success, -1 on failure
 */
int connect_to_device(device *dev, const char *ip, int port, int id) {
    // Validate input parameters
    if (!dev || !ip) return -1;
    
    // Create a new socket for the connection
    int new_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (new_fd == -1) {
        print_error("Socket creation failed");
        return -1;
    }
    
    // Configure address structure
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    // Convert IP from string to binary format
    if (inet_pton(AF_INET, ip, &addr.sin_addr.s_addr) <= 0) {
        print_error("Invalid IP address");
        close(new_fd);
        return -1;
    }

    // Connect to the specified device
    if (connect(new_fd, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        // Successfully connected - update device structure
        dev->fd = new_fd;
        dev->id = id;
        dev->port_num = port;
        strncpy(dev->my_ip, ip, IP_LEN - 1);
        dev->my_ip[IP_LEN - 1] = '\0';  // Ensure null termination
        dev->addr = addr;
        
        printf("Connected to IP: %s, Port: %d\n", ip, port);
        
        // Create a thread to handle receiving messages
        device* new_device = malloc(sizeof(device));
        if (!new_device) {
            print_error("Memory allocation failed");
            close(new_fd);
            return -1;
        }
        
        // Copy device data to thread parameter
        memcpy(new_device, dev, sizeof(device));
        
        // Create a thread to handle incoming messages from this device
        pthread_t recv_thread;
        if (pthread_create(&recv_thread, NULL, receive_message_handler, new_device)) {
            print_error("Cannot create thread to receive message");
            free(new_device);
            close(new_fd);
            return -1;
        }
        
        // Store thread ID and detach it
        dev->recv_thread = recv_thread;
        pthread_detach(recv_thread);
        
        return 0;
    } else {
        print_error("Connection failed");
        close(new_fd);
        return -1;
    }
}

/**
 * Disconnect from a device and close the socket
 * 
 * @param dev Device to disconnect from
 * @return 0 on success, -1 on failure
 */
int disconnect_device(device *dev) {
    // Validate device is active
    if (!dev || dev->fd < 0 || !dev->is_active) {
        return -1;
    }
    
    // Prepare termination message
    char message[100];
    snprintf(message, sizeof(message), "The connection has been terminated by the remote peer");
    
    // Try to send termination message to notify the peer
    send_message(dev, message);
    
    // Close the socket - this will cause the receive thread to exit
    close(dev->fd);
    dev->fd = -1;
    dev->is_active = false;
    
    return 0;
}

/**
 * Disconnect all devices and close their sockets
 * Used during program shutdown
 */
void disconnect_all_devices() {
    pthread_mutex_lock(&device_list_mutex);
    
    // Disconnect all outgoing connections
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (device_connect_to[i].is_active && device_connect_to[i].fd > 0) {
            // Just close and mark as inactive, don't send messages during shutdown
            close(device_connect_to[i].fd);
            device_connect_to[i].fd = -1;
            device_connect_to[i].is_active = false;
        }
    }
    
    // Disconnect all incoming connections
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (device_connect_from[i].is_active && device_connect_from[i].fd > 0) {
            close(device_connect_from[i].fd);
            device_connect_from[i].fd = -1;
            device_connect_from[i].is_active = false;
        }
    }
    
    pthread_mutex_unlock(&device_list_mutex);
}