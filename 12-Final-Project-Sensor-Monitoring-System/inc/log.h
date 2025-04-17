/**
 * @file log.h
 * @brief Interface for the logging system
 */

#ifndef _LOG_H
#define _LOG_H

#include <unistd.h>

pid_t log_start(const char fifo_path, const char *log_file);

#endif

