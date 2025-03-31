#ifndef MESSAGE_H
#define MESSAGE_H

#include "connection.h"

// Message handling functions
int send_message(device dev, const char *message);
void* receive_message_handler(void *arg);

#endif