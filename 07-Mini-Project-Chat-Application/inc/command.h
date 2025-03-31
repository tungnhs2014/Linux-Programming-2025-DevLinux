#ifndef COMMAND_H
#define COMMAND_H

// Command codes
typedef enum {
    CMD_HELP = 1,
    CMD_MYIP,
    CMD_MYPORT,
    CMD_CONNECT,
    CMD_LIST,
    CMD_TERMINATE,
    CMD_SEND,
    CMD_EXIT,
    CMD_INVALID = -1
} Command;

// Command processing functions
void print_command_list();
Command get_command_code(const char *command);
void process_command(char *command_line);

#endif
