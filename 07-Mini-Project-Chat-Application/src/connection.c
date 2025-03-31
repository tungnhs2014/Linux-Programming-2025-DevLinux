#include "connection.h"
#include "message.h"
#include "utils.h"
#include <pthread.h>

extern device this_device;
extern device device_connect_to[MAX_CLIENT];
extern device device_connect_from[MAX_CLIENT];

extern int total_device_to;                   
extern int total_device_from;

extern pthread_t receive_thread_id;

// Initialize socket for the current device
int initialize_socket(device *dev, int port) {
    // Create socket for current device
    dev->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == dev->fd) {
        print_error("Socket createion failed");
        exit(EXIT_FAILURE);
    }

    // Set the SO_REUSEADDR socket option to allow reusing the address
    // This helps avoid "Address already in use" errors when restarting the program
    int opt = 1;
    if (setsockopt(dev->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        print_error("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure the device's address structure with the provided port
    dev->port_num = port;                       // Store the port number
    dev->addr.sin_family = AF_INET;             // Use IPv4 address family
    dev->addr.sin_addr.s_addr = INADDR_ANY;     // Accept connections from any interface
    dev->addr.sin_port = htons(dev->port_num);  // Covert port to network byte order
    
    // Blind the socket to the device's IP address and port
    if (-1 == bind(dev->fd, (struct sockaddr*)&dev->addr, sizeof(dev->addr))) {
        print_error("Blind failed");
        exit(EXIT_FAILURE);
    }

    // Set the socket to listening mode to accept incoming connections
    if (-1 == listen(dev->fd, MAX_SERVER)) {
        print_error("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Listening on port: %d\n", dev->port_num);
    return 0;
}

// Thread function for handling incoming connections
void *accept_connection_handler(void *args) {
    int client_fd;
    struct sockaddr_in client_addr;  // Structure to hold client address information
    
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        // Accept a new incoming connection
        client_fd = accept(this_device.fd, (struct sockaddr *)&client_addr, &client_len);
        if (-1 == client_fd) {
            print_error("Accept new device failed");
            return NULL;
        }

        // Save newly connected device information
        device_connect_from[total_device_from].fd = client_fd;          // Store socket descriptor
        device_connect_from[total_device_from].id = total_device_from;   // Assign a unique ID
        device_connect_from[total_device_from].addr = client_addr;      // Store client address
        device_connect_from[total_device_from].port_num = ntohs(client_addr.sin_port); // / Get port in host byte order

        // Convert the binary IP address to a string representation
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, device_connect_from[total_device_from].my_ip, IP_LEN);

        // Print information about the new connection
        printf("\nNew connection from IP: %s, Port: %d\n", device_connect_from[total_device_from].my_ip, device_connect_from[total_device_from].port_num);
        
        // Create a thread to receive messages from newly connected device
        if (pthread_create(&receive_thread_id, NULL, receive_message_handler, &device_connect_from[total_device_from])) {
        print_error("Can not create thread to recived message"); 
        }
        total_device_from++;
        
    }
    pthread_join(receive_thread_id, NULL);
}   

/*
 * Prints a formatted list of connected devices
 * 
 * This function displays all active devices from the provided device list.
 * It shows each device's ID, IP address, and port number in a tabular format.
 * Only devices with a valid file descriptor (fd > 0) are displayed.
 */
void print_device_list(device *device_list, int total_devices) {
    printf("Device connection\n");
    printf("ID |        IP Address         | Port No.\n");

    for (int i = 0; i < MAX_CLIENT; i++) {
        // Only display devices with valid file descriptors
        if (device_list[i].fd > 0) {
            printf("ID: %d | IP: %s | Port: %d\n", device_list[i].id, device_list[i].my_ip, device_list[i].port_num);
        }
    }

    printf("*****************************************\n");
}


// Connect to another device
int connect_to_device(device *dev, const char *ip, int port, int id) {
    dev->fd = socket(AF_INET, SOCK_STREAM, 0);
    dev->id = id;
    dev->port_num = port;
    strncpy(dev->my_ip, ip, IP_LEN -1);
    dev->my_ip[IP_LEN - 1] = '\0'; // Ensure null-termination

    dev->addr.sin_family = AF_INET;
    dev->addr.sin_port = htons(dev->port_num);
    inet_pton(AF_INET, dev->my_ip, &dev->addr.sin_addr.s_addr);

    if (connect(dev->fd, (struct sockaddr*)&dev->addr, sizeof(dev->addr)) == 0) {
        printf("Connected to IP: %s, Port: %d\n", ip, port);
        return 0;
    }
    else {
        print_error("Connection failed");
        return -1;
    }
}

// Disconnect from a device
int disconnect_device(device *dev) {
    char message[100];

    // Notify other device that the connection has been terminated
    snprintf(message, sizeof(message), "The connection at port %d has just been terminated\n", dev->port_num);
    send_message(*dev, message);

    dev->fd = -1; // Mark as disconnected
    return 0;
}

// Disconnect from all device
void disconnect_all_device(device *device_list, int total_decvices) {
    for (int i = 0; i < total_decvices; i++) {
        if(device_list[i].fd > 0) {
            disconnect_device(&device_list[i]);
        }
    }
}