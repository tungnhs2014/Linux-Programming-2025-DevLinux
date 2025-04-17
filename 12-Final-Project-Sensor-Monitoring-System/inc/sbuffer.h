/**
 * @file sbuffer.h
 * @brief Shared buffer interface for the Sensor Monitoring System
 * 
 */
#ifndef _SBUFFER_H_
#define _SBUFFER_H_

#include "config.h"

/**
 * Opaque data strucure for the shared buffer
 */
typedef struct sbuffer sbuffer_t;

/**
 * Return value for sbuffer operations
 */
typedef enum {
    SBUFFER_SUCESS = 0,   // Operation completed sucessfully
    SBUFFER_NO_DATA,      // No data available in the buffer
    SBUFFER_FAILURE
} sbuffer_result_t;

/**
 * Consumer ID for identifying which thread is accessing the buffer
 */
#define NUM_CONSUMERS   2  // Total number of consumers

/**
 * @brief Initialize a new shared buffer
 *
 * @return Pointer to the new buffer or NULL if creation failed
 */
sbuffer_t *sbuffer_init(void);

/**
 * @brief Free all resource used by the shared buffer
 * 
 * @param buffer Pointer to the buffer to free 
 */
void sbuffer_free(sbuffer_t **buffer);

#endif