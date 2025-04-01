#include "command.h"
#include "connection.h"
#include "message.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

extern device this_device;
extern device device_connect_to[MAX_CLIENT];
extern int total_device_to;
extern pthread_t accept_thread_id;

// Get numeric code for command
Command get_command_code(const char *command) {
    if (!strcmp(command, "help")) {
        return CMD_HELP;
    }
    if (!strcmp(command, "myip")) {
        return CMD_MYIP;
    }
    if (!strcmp(command, "myport")) {
        return CMD_MYPORT;
    }
    if (!strcmp(command, "connect")) {
        return CMD_CONNECT;
    }
    if (!strcmp(command, "list")) {
        return CMD_LIST;
    }
    if (!strcmp(command, "terminate")) {
        return CMD_TERMINATE;
    }
    if (!strcmp(command, "send")) {
        return CMD_SEND;
    }
    if (!strcmp(command, "exit")) {
        return CMD_EXIT;
    }

    return CMD_INVALID;
}

// Print the list of available command
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

// Process a command entered by the user
void process_command(char *command_line) {
    char command_option[10];
    sscanf(command_line, "%s", command_option);  // Sao chép lệnh vào command_option

    switch(get_command_code(command_option)) {
        case CMD_HELP:
            print_command_list();
            break;

        case CMD_MYIP:
            print_my_ip(this_device.my_ip);
            break;

        case CMD_MYPORT:
            print_my_port(this_device.port_num);
            break;

        case CMD_CONNECT: {
            char ip[IP_LEN];
            int port;
            if (sscanf(command_line, "%*s %s %d", ip, &port) < 2) {
                print_error("Invalid input: Usage: connect <ip> <port>");
                break;
            }

            if (connect_to_device(&device_connect_to[total_device_to], ip, port, total_device_to) == 0) {
                total_device_to++;
            }
            break;
        }
            
        case CMD_LIST:
            print_device_list(device_connect_to, total_device_to);
            break;
        
        case CMD_SEND: {
            char message[256];
            int id;

            if (sscanf(command_line, "%*s %d %[^\n]", &id, message) < 2) {
                print_error("Invalid input. Usage: send <id> <message>");
                break;
            }
            
            int found = 0;
            for (int i = 0; i < total_device_to; i++) {
                if (id == device_connect_to[i].id) {
                    if(send_message(device_connect_to[i], message) == 0) {
                        print_error("FAILED to send message");
                    }
                    found = 1;
                }
            }
            if (!found) {
                printf("ERROR: Device with ID %d not found.\n", id);
            }

            break;
        }

        case CMD_TERMINATE: {
            int id; 
            if (sscanf(command_line, "%*s %d", &id) < 1) {
                print_error("Invalid input. Usage: terminate <id>");
                break;
            }
            for (int i = 0; i < total_device_to; i++) {
                if (id == device_connect_to[i].id) {
                    if(disconnect_device(&device_connect_to[i])) {
                        print_error("FAILED to disconnect device");
                        break;
                    }
                }
            }
            break;
        }
        
        case CMD_EXIT:
            disconnect_all_device(device_connect_to, total_device_to);
            printf("**************************************************************************\n");
            printf("-----------------------ENDING PROGRAM-------------------------------------\n");
            printf("**************************************************************************\n");
            pthread_cancel(accept_thread_id);
            pthread_join(accept_thread_id, NULL);
            exit(0);
            break;
            
        default:
            printf("INVALID command.\n");
            break;
    }
}