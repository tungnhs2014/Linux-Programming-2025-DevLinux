#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>
#include <stdbool.h>

/**
 * Function Declarations for Utility Functions
*/
void print_error(const char *message);                  // Print an error message to stderr
void signal_handler();                                  // Signal handler for shutting down the program on signals like SIGINT
bool get_my_ip(char *ip_buffer, size_t buffer_len);     // Get the current device's IP address
void print_my_ip(char *ip_buffer);                      // Display the current device's IP address
void print_my_port(int port);                           // Display the current device's listening port
void clear_input_buffer();                              // Clear the input buffer to avoid issues with fgets
bool is_valid_ip(const char *ip);                       // Validate if a string is a valid IP address
void cleanup_resources();                                // Clean up all resources before exiting

#endif