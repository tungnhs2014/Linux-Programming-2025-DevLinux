#include "message.h"
#include "utils.h"

// Thread function to handle receiving messages
void* receive_message_handler(void *arg) {
    device *recv_dev = (device *)arg;
    char buffer[256];
    
    while (1) {
        // Read data from device via socket
        ssize_t byte_read = read(recv_dev->fd, buffer, sizeof(buffer) -1);
        if (byte_read < 0) {
            print_error("Can not read data");
            return NULL;
        }

        // Ensure null terminated string
        buffer[byte_read] = '\0';

        if (recv_dev->fd > 0) {
            printf("***Message received from: %s\n", recv_dev->my_ip);
            printf("***Sender Port:          %d\n", recv_dev->port_num);
            printf("-->Message:              %s\n", buffer);
        }
        else {
            printf("Notification: %s\n", buffer);
        }
    }
    
}   