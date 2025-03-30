#ifndef COMMAND_H
#define COMMAND_H

// Command codes
typedef enum {
    CMD_HELP = 1,
    CMD_MYIP,
    CMD_MYPORT,
    CMD_CONNECT,
    CMD_LIST,
    CMD_SEND,
    CMD_TERMINATE,
    CMD_EXIT,
    CMD_INVALID = -1
} command;

// Command processing functions
void print_command_list();

#endif
