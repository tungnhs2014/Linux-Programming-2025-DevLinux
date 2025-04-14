#ifndef MESSAGE_H
#define MESSAGE_H

#include "connection.h"

/*  Constants for message handling */
#define MAX_MESSAGE_LEN 100

/** 
 * Function declarations for sending and receiving messages
*/
int send_message(device *dev, const char *message);
void* receive_message_handler(void *arg);

#endif