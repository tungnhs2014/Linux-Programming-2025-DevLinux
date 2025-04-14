#include "message.h"
#include "utils.h"
#include <string.h>
#include <pthread.h>
#include <errno.h>

/* External variables */
extern device device_connect_to[MAX_CLIENT];
extern device device_connect_from[MAX_CLIENT];
extern pthread_mutex_t device_list_mutex;

/**
 * Send a message to a connected device
 * 
 * @param dev Device to send message to
 * @param message Message string to send
 * @return 1 on success, 0 on failure
 */
int send_message(device *dev, const char *message) {
    // Validate input parameters
    if (!dev || !message) return 0;
    
    // Check message length to ensure it doesn't exceed max
    size_t message_len = strlen(message);
    if (message_len > MAX_MESSAGE_LEN) {
        print_error("Message exceeds maximum length");
        return 0;
    }
    
    // Local variables to use after mutex is released
    int fd;
    bool active;
    
    // Critical section - just check status and get file descriptor
    pthread_mutex_lock(&device_list_mutex);
    fd = dev->fd;
    active = dev->is_active;
    pthread_mutex_unlock(&device_list_mutex);
    
    // Check if device is still connected
    if (fd < 0 || !active) {
        print_error("This device has been disconnected");
        return 0;
    }
    
    // Write the message to the device's socket
    ssize_t bytes_sent = write(fd, message, message_len);
    
    // Check for write errors
    if (bytes_sent < 0) {
        print_error("Cannot send message");
        return 0;
    }
    
    // Check if full message was sent
    if ((size_t)bytes_sent < message_len) {
        print_error("Partial message sent");
        return 0;
    }
    
    return 1;  // Return success if message is sent
}

/**
 * Thread function to handle receiving messages from a connected device
 * 
 * @param arg Pointer to device structure for this connection
 * @return NULL when thread exits
 */
void* receive_message_handler(void *arg) {
    // Cast and validate the device pointer
    device *recv_dev = (device *)arg;
    if (!recv_dev) {
        print_error("Invalid device data in receive handler");
        return NULL;
    }
    
    // Buffer for receiving messages
    char buffer[MAX_MESSAGE_LEN + 1];
    
    // Store connection info locally to avoid race conditions
    int fd = recv_dev->fd;
    char ip[IP_LEN];
    int port = recv_dev->port_num;
    
    // Copy device info locally to avoid race conditions
    strncpy(ip, recv_dev->my_ip, IP_LEN);
    ip[IP_LEN - 1] = '\0';  // Ensure null termination
    
    // Free the malloc'd structure since we've copied the needed data
    free(recv_dev);
    
    // Main message receiving loop
    while (1) {
        // Clear buffer before each read
        memset(buffer, 0, sizeof(buffer));
        
        // Read incoming message from the device's socket
        ssize_t byte_read = read(fd, buffer, MAX_MESSAGE_LEN);
        
        // Check for errors or connection close
        if (byte_read <= 0) {
            // Check if interrupted by signal
            if (byte_read < 0 && errno == EINTR) {
                // Interrupted by signal, continue loop
                continue;
            }
            
            // Connection closed or error occurred - find and update device status
            bool found = false;
            
            pthread_mutex_lock(&device_list_mutex);
            
            // Check outgoing connections
            for (int i = 0; i < MAX_CLIENT; i++) {
                if (device_connect_to[i].fd == fd) {
                    device_connect_to[i].is_active = false;
                    device_connect_to[i].fd = -1;
                    found = true;
                    break;
                }
            }
            
            // Check incoming connections if not found in outgoing
            if (!found) {
                for (int i = 0; i < MAX_CLIENT; i++) {
                    if (device_connect_from[i].fd == fd) {
                        device_connect_from[i].is_active = false;
                        device_connect_from[i].fd = -1;
                        found = true;
                        break;
                    }
                }
            }
            
            pthread_mutex_unlock(&device_list_mutex);
            
            // Print appropriate message
            if (found) {
                printf("\nConnection with %s:%d has been closed\n", ip, port);
            } else {
                printf("\nDisconnected from unknown device\n");
            }
            
            // Ensure prompt is redisplayed
            printf("\nEnter command: ");
            fflush(stdout);
            
            // Close socket and exit thread
            close(fd);
            break;
        }
        
        // Ensure null-termination of the received message
        buffer[byte_read] = '\0';
        
        // Print the received message with sender info
        printf("\n");
        printf("***Message received from: %s\n", ip);
        printf("***Sender Port:          %d\n", port);
        printf("-->Message:              %s\n", buffer);
        printf("\nEnter command: ");
        fflush(stdout);  // Make sure the prompt is displayed
    }
    
    return NULL;
}