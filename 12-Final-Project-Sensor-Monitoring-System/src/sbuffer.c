/** 
 * @file sbuffer.c
 * @brief Implementation of the shared buffer
*/

#include "sbuffer.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/**
 * Structure representing a node in the buffer's linked list
 */
typedef struct sbuffer_node {
    sensor_data_t data;           // The sensor data
    struct sbuffer_node *next;    // Pointer to the next node
    int consumed[NUM_CONSUMERS];  // FLag to track which consumers processed this data
} sbuffer_node_t;

/**
 * Structure represeting the shared buffer 
 */
struct sbuffer {
    sbuffer_node_t *head;            // Pointer to thr first node (oldest data)
    sbuffer_node_t *tail;            // Pointer to the last mode (newest data)
    pthread_mutex_t mutex;          // Mutex for thread-safe acess
    pthread_cond_t data_available;  // Condition variable to singal when data is available
    int size;                       // Current number of items in the buffer
};

sbuffer_t *sbuffer_init(void) {
    // Allocate memory for the buffer
    sbuffer_t *buffer = malloc(sizeof(sbuffer_t));
    if (buffer == NULL) {
        return NULL;
    }

    // Initialize buffer fields
    buffer->head = NULL;
    buffer->tail = NULL;
    buffer->size = 0;

    // Initialzie the mutex;
    if (pthread_mutex_init(&buffer->mutex, NULL) != 0) {
        free(buffer);
        return NULL;
    }

    // Initialize the condition variable
    if (pthread_cond_init(&buffer->data_available, NULL) != 0) {
        pthread_mutex_destroy(&buffer->mutex);
        free(buffer);
        return NULL;   
    }

    return buffer;
}

void sbuffer_free(sbuffer_t **buffer) {
    if (buffer == NULL || *buffer == NULL) {
        return;
    }

    // Lock the mutex to ensure safe destruction
    pthread_mutex_lock(&(*buffer)->mutex);

    // Free all node in the buffer
    sbuffer_node_t *current = (*buffer)->head;
    while (current != NULL) {
        sbuffer_node_t *temp = current;
        current = current->next;
        free(temp);
    }

    // Unlock the mutex
    pthread_mutex_unlock(&(*buffer)->mutex);

    // Destroy synchronization primitives
    pthread_mutex_destroy(&(*buffer)->mutex);
    pthread_cond_destroy(&(*buffer)->data_available);

    // Free the buffer structure and set pointer to NULL
    free(*buffer);
    *buffer = NULL;
}
