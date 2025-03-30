#include "command.h"
#include "utils.h"
#include <stdio.h>

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