/**
 * @file config.h
 * @brief Configuration parameters for the Sensor Monitoring System
*/


#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>

/* General configuration */
#define LOG_FIFO_PATH       "logfifo"           // Path for the log FIFO
#define LOG_FILE_PATH       "gateway.log"       // Path for the log file
#define MAX_LOG_LENGTH      256                 // Maximum length of a log message

/*Room and sensor setting */
#define MAX_ROM_ID_LENGTH   16                  // Maximum length for rom ID string

/* Sensor data structure */
typedef struct {    
    uint16_t id;                    // Sensor iD
    time_t ts;                      // Timestamp
    double value;                   // Temperature value
    char rom_id[MAX_ROM_ID_LENGTH]; // Room identifier  
} sensor_data_t;

/* Sensor connection info structure */
typedef struct {
    int socket_id;                  // Socket file descriptor
    uint16_t sensor_id;             // Sensor ID
    char ip_address[16];            // IP address of the connected sensor
    uint16_t port;                  // Port of tthe connected sensor
    time_t last_active;             // Last activity timestamp  
} sensor_conn_t;

#endif 