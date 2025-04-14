#include "command.h"
#include "connection.h"
#include "message.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

/* External variables from other modules */
extern device this_device;
extern device device_connect_to[MAX_CLIENT];
extern device device_connect_from[MAX_CLIENT];
extern int total_device_to;
extern int total_device_from;
extern pthread_mutex_t device_list_mutex;

/**
 * Get numeric code for command based on the user input
 * Maps string command to enum value for easier handling
 * 
 * @param command The command string entered by user
 * @return Command code enum value
 */
Command get_command_code(const char *command) {
    // Validate input
    if (!command || !*command) return CMD_INVALID;
    
    // Match command string to enum value
    if (!strcmp(command, "help")) return CMD_HELP;
    if (!strcmp(command, "myip")) return CMD_MYIP;
    if (!strcmp(command, "myport")) return CMD_MYPORT;
    if (!strcmp(command, "connect")) return CMD_CONNECT;
    if (!strcmp(command, "list")) return CMD_LIST;
    if (!strcmp(command, "terminate")) return CMD_TERMINATE;
    if (!strcmp(command, "send")) return CMD_SEND;
    if (!strcmp(command, "exit")) return CMD_EXIT;
    return CMD_INVALID; // Return invalid if no match found
}

/**
 * Print the list of available commands
 * Shows all supported commands with descriptions
 */
void print_command_list() {
    printf("************************Command List*************************\n");
    printf("help                         : Display all commands\n");
    printf("myip                         : Display IP of this device\n");
    printf("myport                       : Display port of this device\n");
    printf("connect <ip> <port_num>      : Connect to device with IP and port\n");
    printf("list                         : Display all connected devices\n");
    printf("send <id> <message>          : Send message to device with id\n");
    printf("terminate <id>               : Disconnect with device at id\n");
    printf("exit                         : Close application\n");
    printf("************************************************************\n");
}

/**
 * Process a command entered by the user
 * Parses the command and executes the corresponding action
 * 
 * @param command_line Full command line entered by user
 */
void process_command(char *command_line) {
    // Validate input
    if (!command_line || !*command_line) return;

    // Extract command from input line
    char command_option[20] = {0};
    int result = sscanf(command_line, "%19s", command_option);
    if (result != 1) {
        printf("Invalid command format.\n");
        return;
    }

    // Process command based on its type
    switch(get_command_code(command_option)) {
        case CMD_HELP: 
            // Display available commands
            print_command_list(); 
            break;
            
        case CMD_MYIP: 
            // Display this device's IP address
            print_my_ip(this_device.my_ip); 
            break;
            
        case CMD_MYPORT: 
            // Display this device's port number
            print_my_port(this_device.port_num); 
            break;
            
        case CMD_CONNECT: {
            // Parse IP and port from command
            char ip[IP_LEN] = {0};
            int port;
            if (sscanf(command_line, "%*s %49s %d", ip, &port) < 2) {
                print_error("Invalid input: Usage: connect <ip> <port>");
                break;
            }
            
            // Validate IP address format
            if (!is_valid_ip(ip)) {
                print_error("Invalid IP address format");
                break;
            }
            
            // Check if port is in valid range
            if (port <= 0 || port > 65535) {
                print_error("Invalid port number. Port must be between 1 and 65535");
                break;
            }
            
            // Check for self-connection attempt
            if (strcmp(ip, this_device.my_ip) == 0 && port == this_device.port_num) {
                print_error("Self-connection is not allowed");
                break;
            }
            
            // Check for duplicate connections and find available slot
            bool duplicate = false;
            int idx = -1;
            
            pthread_mutex_lock(&device_list_mutex);
            
            // Check for duplicates
            for (int i = 0; i < MAX_CLIENT; i++) {
                if (device_connect_to[i].is_active && 
                    strcmp(device_connect_to[i].my_ip, ip) == 0 && 
                    device_connect_to[i].port_num == port) {
                    duplicate = true;
                    break;
                }
            }
            
            // If not duplicate, find available slot
            if (!duplicate) {
                for (int i = 0; i < MAX_CLIENT; i++) {
                    if (!device_connect_to[i].is_active) {
                        idx = i;
                        break;
                    }
                }
            }
            
            pthread_mutex_unlock(&device_list_mutex);
            
            // Handle errors
            if (duplicate) {
                print_error("Duplicate connection is not allowed");
                break;
            }
            
            if (idx == -1) {
                print_error("Maximum connections reached");
                break;
            }
            
            // Connect to the device
            if (connect_to_device(&device_connect_to[idx], ip, port, idx) == 0) {
                pthread_mutex_lock(&device_list_mutex);
                device_connect_to[idx].is_active = true;
                total_device_to++;
                pthread_mutex_unlock(&device_list_mutex);
            }
            break;
        }
        
        case CMD_LIST: 
            // Display list of connected devices
            print_device_list(); 
            break;
            
        case CMD_SEND: {
            // Parse message parameters
            char message[MAX_MESSAGE_LEN + 1] = {0};
            int id;
            if (sscanf(command_line, "%*s %d %100[^\n]", &id, message) < 2) {
                print_error("Invalid input. Usage: send <id> <message>");
                break;
            }
            
            // Check message length
            if (strlen(message) > MAX_MESSAGE_LEN) {
                print_error("Message too long. Maximum message length is 100 characters");
                break;
            }
            
            // Find the device with the specified ID
            device *target_device = NULL;
            
            pthread_mutex_lock(&device_list_mutex);
            
            if (id >= MAX_CLIENT) {
                // Handle incoming connections (device_connect_from)
                int idx = id - MAX_CLIENT;
                if (idx >= 0 && idx < MAX_CLIENT && device_connect_from[idx].is_active) {
                    target_device = &device_connect_from[idx];
                }
            } else {
                // Handle outgoing connections (device_connect_to)
                for (int i = 0; i < MAX_CLIENT; i++) {
                    if (device_connect_to[i].is_active && device_connect_to[i].id == id) {
                        target_device = &device_connect_to[i];
                        break;
                    }
                }
            }
            
            pthread_mutex_unlock(&device_list_mutex);
            
            if (!target_device) {
                printf("ERROR: Device with ID %d not found or not active.\n", id);
                break;
            }
            
            // Send message
            if(send_message(target_device, message)) {
                printf("Message sent to device with ID: %d\n", id);
            } else {
                print_error("Failed to send message");
            }
            break;
        }
        
        case CMD_TERMINATE: {
            // Parse connection ID to terminate
            int id; 
            if (sscanf(command_line, "%*s %d", &id) < 1) {
                print_error("Invalid input. Usage: terminate <id>");
                break;
            }
            
            // Find the device with the specified ID
            device *target_device = NULL;
            
            pthread_mutex_lock(&device_list_mutex);
            
            if (id >= MAX_CLIENT) {
                // Handle incoming connections (device_connect_from)
                int idx = id - MAX_CLIENT;
                if (idx >= 0 && idx < MAX_CLIENT && device_connect_from[idx].is_active) {
                    target_device = &device_connect_from[idx];
                }
            } else {
                // Handle outgoing connections (device_connect_to)
                for (int i = 0; i < MAX_CLIENT; i++) {
                    if (device_connect_to[i].is_active && device_connect_to[i].id == id) {
                        target_device = &device_connect_to[i];
                        break;
                    }
                }
            }
            
            pthread_mutex_unlock(&device_list_mutex);
            
            if (!target_device) {
                printf("ERROR: Device with ID %d not found or already disconnected.\n", id);
                break;
            }
            
            // Terminate the connection
            if(disconnect_device(target_device) == 0) {
                printf("Connection with ID %d terminated successfully.\n", id);
            } else {
                print_error("Failed to disconnect device");
            }
            break;
        }
        
        case CMD_EXIT: {
            // Exit the application
            printf("Exiting application...\n");
            cleanup_resources();
            exit(EXIT_SUCCESS);
            break;
        }
        
        default: 
            // Handle invalid command
            printf("Invalid command. Type 'help' for a list of available commands.\n"); 
            break;
    }
}