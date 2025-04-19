/**
 * @file connmgr.h
 * @brief Connection Manager interface
 */

 #ifndef _CONNMGR_H_
 #define _CONNMGR_H_

#include "sbuffer.h"

/**
 * @brief Start the Connection Manager thread
 * 
 * This function initializes and starts the Connection Manager thread
 * that listens for incomming TCP connection from sensor nodes and
 * forwards sensor data to the shared buffer.
 * 
 * @param port Port number to listen to
 * @param buffer Pointer to the shared buffer
 * @param log_fd File descriptor for the log FIFO
 * @return 0 on sucess, -1 on failure
 */
int connmgr_star(int port, sbuffer_t *buffer, int log_fd);

/**
 * @brief Stop the Connection Manager thread
 * 
 * This function signals the Connection Manager thread to stop
 * and cleans up associated resources.
 * 
 * @return 0 on success, -1 on failure
 */
int connmgr_stop(void);

 #endif