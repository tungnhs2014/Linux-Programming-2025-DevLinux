# Sensor Monitoring System

## Overview

The Sensor Monitoring System is a multi-threaded, multi-process gateway application designed to monitor room temperatures through a network of sensor nodes. It collects temperature data via TCP connections, processes the readings to detect conditions (too hot/cold), and stores all data in an SQLite database for future analysis.

This system demonstrates advanced concepts in system programming including:
- Multi-threading and process management
- Thread synchronization and data sharing
- Network programming with TCP sockets
- Database operations with SQLite
- Inter-process communication via FIFOs (named pipes)
- Signal handling and resource management

## System Architecture

The system consists of three main components:

1. **Sensor Nodes**: Physical or simulated devices that measure room temperature and transmit data to the gateway using TCP connections.

2. **Sensor Gateway**: The core of the system with a multi-process, multi-threaded architecture:
   - **Main Process**: Manages three worker threads and the shared buffer
   - **Log Process**: Handles system-wide logging (runs as a separate process)

3. **SQLite Database**: Stores all sensor data for long-term persistence and analysis

## Gateway Architecture

The Sensor Gateway implements a sophisticated architecture:

### Main Process
Contains three primary threads:
- **Connection Manager (connmgr)**: Handles TCP socket connections from sensor nodes, receives data packets
- **Data Manager (datamgr)**: Processes temperature readings, calculates running averages, detects temperature conditions
- **Storage Manager (storagemgr)**: Writes sensor data to the SQLite database

### Log Process
- Spawned by the main process using `fork()`
- Receives log events via a FIFO (named pipe)
- Formats and writes log entries to a log file
- Provides sequence numbers and timestamps for each log entry

### Shared Buffer
- Thread-safe data structure for passing sensor data between threads
- Uses mutex locks and condition variables for synchronization
- Implements a multi-consumer model where data is only removed when processed by all consumers

## Directory Structure

```
sensor_monitoring_system/
├── bin/                 # Compiled binary files
├── include/             # Header files
│   ├── config.h         # Configuration parameters
│   ├── connmgr.h        # Connection manager interface
│   ├── datamgr.h        # Data manager interface
│   ├── log.h            # Log process interface
│   ├── sbuffer.h        # Shared buffer interface
│   └── sensor_db.h      # Storage manager interface
├── obj/                 # Object files
├── src/                 # Source files
│   ├── connmgr.c        # Connection manager implementation
│   ├── datamgr.c        # Data manager implementation
│   ├── log.c            # Log process implementation
│   ├── main.c           # Main program
│   ├── sbuffer.c        # Shared buffer implementation
│   └── sensor_db.c      # Storage manager implementation
├── Makefile             # Build configuration
└── README.md            # Project documentation
```

## Building and Running

### Prerequisites
- GCC compiler
- SQLite3 development libraries (`libsqlite3-dev`)
- POSIX-compliant system (Linux recommended)

### Build Instructions
```bash
# Clone the repository
git clone https://github.com/yourusername/sensor-monitoring-system.git
cd sensor-monitoring-system

# Create the room-sensor mapping file
make setup

# Build the project
make

# Run with a specified port
./bin/sensor_gateway 5678
```

### Memory Check with Valgrind
```bash
make memcheck
```

## Implementation Details

### Shared Buffer

The shared buffer is implemented as a thread-safe linked list with the following features:
- Mutex lock for synchronized access
- Condition variable to signal when data is available
- Multi-consumer design where data is only removed after all consumers have processed it
- Consumer-specific tracking of processed data

### Connection Manager

- Uses `select()` for managing multiple client connections
- Handles connection timeouts and graceful disconnection
- Parses incoming sensor data packets and forwards them to the shared buffer

### Data Manager

- Loads sensor-to-room mappings from a configuration file
- Maintains a running average of temperature for each sensor
- Detects and logs "too hot" or "too cold" conditions based on thresholds
- Associates room IDs with sensor readings for context

### Storage Manager

- Creates and maintains the SQLite database schema
- Stores sensor readings with room context
- Handles database connection failures with automatic retry logic
- Provides statistics on stored records

### Log Process

- Runs as a separate process from the main application
- Receives log messages via a FIFO (named pipe)
- Formats log entries with sequence numbers and timestamps
- Writes to a persistent log file for later analysis

## Key Requirements Fulfillment

### Functional Requirements
1. ✅ Connection Management: Handles multiple concurrent connections with session tracking
2. ✅ Message Sending/Receiving: Processes sensor data packets efficiently
3. ✅ System Status Management: Provides real-time status information
4. ✅ Exit and Recovery: Implements graceful shutdown and error recovery

### Non-Functional Requirements
1. ✅ Performance: Uses efficient I/O multiplexing and resource management
2. ✅ Reliability: Implements fault tolerance and retry mechanisms
3. ✅ Memory Management: Prevents memory leaks with proper resource cleanup
4. ✅ Code Quality: Modular design with clean separation of concerns