#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Print error message
void print_error(const char *message){
    fprintf(stderr, "ERROR %s\n", message);
}

// Handle SIGINT signal
void signal_handler() {
    printf("***********************************************************\n");
    printf("-----------------Program terminated!-----------------------\n");
    printf("***********************************************************\n");
    exit(EXIT_SUCCESS);
}
