#ifndef COMMAND_H
#define COMMAND_H

/** 
 * Command codes enumertions
 *  This defines all the commands that the user can input into the CLI
*/
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

/* Function declarations for handling commands */ 
void print_command_list();                              // Display all available commands to the user
Command get_command_code(const char *command);          // Map user input to corresponding command code
void process_command(char *command_line);               // Process and execute the user's command

#endif
