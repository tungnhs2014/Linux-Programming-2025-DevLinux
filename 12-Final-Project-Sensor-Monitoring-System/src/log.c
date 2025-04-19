/**
 * @file log.c
 * @brief Implementaion of the logging system
 */

#include "log.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

// Flag to control log process execution
static volatile int running = 1;

/**
 * Signal handler for the log process
 */
static void sig_handler (int signo) {
    if (signo == SIGTERM || signo == SIGINT) {
        running = 0;
    }
}

/**
 * Get current timestamp as string
 */
static char *get_timestamp(void) {
    static char timestamp[32];
    time_t now = time(NULL);
    struct tm *tim_info = localtime(&now);

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tim_info);

    return timestamp;
}

/**
 * Log process main function
 */
 static void log_process_func(const char *fifo_path, const char *log_file) {
    int fifo_fd, log_fd;
    char buffer[MAX_LOG_LENGTH];
    ssize_t bytes_read;

    // Open the file log
    log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    // Open the FIFO for reading
    fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd < 0) {
        perror("Failed to open FIFO for reading");
        close(log_fd);
        exit(EXIT_FAILURE);
    }

    // Write initial log entry
    char initial_log[MAX_LOG_LENGTH];
    snprintf(initial_log, sizeof(initial_log), "0 %s Log process started\n", get_timestamp());
    write(log_fd, initial_log, strlen(initial_log));

    // Sequence number for Log entries
    int seq_num = 1;

    // Main Log processing loop
    while (running) {
        // Read a message from the FIFO
        bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1);

        if (bytes_read > 0) {
            // Null-terminate the message
            buffer[bytes_read] = '\0';

            // Format the Log entry with sequence number and timestamp
            char log_entry[MAX_LOG_LENGTH * 2];
            snprintf(log_entry, sizeof(log_entry), "%d %s %s\n", 
                seq_num++, get_timestamp(), buffer);

            write(log_fd, log_entry, strlen(log_entry));
        }
        else if (bytes_read < 0 && errno != EAGAIN) {
            // Check for errors reading from FIFO, ignoring EAGAIN (no data available)
            perror("Error reading from FIFO");
            break;
        }
        else {
            // No data available, sleep for a short time
            usleep(100000); // 100ms
        }
    }

    // Write final log entry
    char final_log[MAX_LOG_LENGTH];
    snprintf(final_log, sizeof(final_log), "%d %s Log process stopped\n",
            seq_num, get_timestamp());
    write(log_fd, final_log, strlen(final_log));

    // Close file descriptors
    close(fifo_fd);
    close(log_fd);
}

pid_t log_start(const char *fifo_path, const char *log_file) {
    if (fifo_path == NULL || log_file == NULL) {
        return -1;
    }

    // Create the FIFO if it doesn't exist
    if (access(fifo_path, F_OK) == -1) {
        if (mkfifo(fifo_path, 0666) != 0) {
            perror("Failed to create FIFO");
            return -1;
        }
    }

    // Fork a chill process for logging
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork faield");
        return -1;
    }  
    else if (pid == 0) {
        // Child process

        // Set up signal handlers
        signal(SIGTERM, sig_handler);
        signal(SIGINT, sig_handler);

        // Run the Log process function
        log_process_func(fifo_path, log_file);

        // Exit thje child process
        exit(EXIT_FAILURE);
    }

    // Return pid child process ID
    return pid;
}

int log_open_fifo(const char *fifo_path) {
    if (fifo_path == NULL) {
        return -1;
    }
    
    // Create the FIFO if it doesn't exist
    if (access(fifo_path, F_OK) == -1) {
        if (mkfifo(fifo_path, 0666) != 0) {
            perror("Failed to create FIFO");
            return -1;
        }
    }

    // Open the FIFO for writiing
    int fd = open(fifo_path, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Faild to open FIFO for writing");
        return -1;
    }

    return fd;
}