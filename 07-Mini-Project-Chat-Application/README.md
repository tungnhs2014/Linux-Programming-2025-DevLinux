# Chat Application

## Overview
This is a command-line chat application that enables peer-to-peer messaging through TCP connections. The application allows users to establish, manage, and terminate connections with multiple peers simultaneously while exchanging real-time messages.

## Features
- Peer-to-peer TCP socket communication
- Support for both outgoing and incoming connections
- Concurrent handling of multiple connections using threads
- Real-time message exchange
- Simple command-line interface
- Clean resource handling and memory management

## Project Structure
```
.
├── bin                 # Binary executables
│   └── chat_app        # Main application executable
├── inc                 # Header files
│   ├── command.h       # Command processing definitions
│   ├── connection.h    # Network connection handling
│   ├── message.h       # Message structure and functions
│   └── utils.h         # Utility functions
├── log                 # Log files
│   └── valgrind.log    # Memory leak check logs
├── Makefile            # Build configuration
├── obj                 # Compiled object files
├── README.md           # This file
└── src                 # Source code
    ├── chat.c          # Main application entry point
    ├── command.c       # Command processing implementation
    ├── connection.c    # Connection management implementation
    ├── message.c       # Message handling implementation
    └── utils.c         # Utility functions implementation
```

## Command List
The application supports the following commands:

| Command | Description |
|---------|-------------|
| `help` | Display all available commands |
| `myip` | Display IP address of this device |
| `myport` | Display port on which this device is listening |
| `connect <ip> <port>` | Connect to a remote device at the specified IP and port |
| `list` | Display all active connections |
| `send <id> <message>` | Send a message to the device with the specified ID |
| `terminate <id>` | Disconnect from the device with the specified ID |
| `exit` | Close all connections and exit the application |

## Implementation Details

### Connection Management
- Each connection is tracked in either `device_connect_to` (outgoing) or `device_connect_from` (incoming) arrays
- Connections are managed using thread-safe operations with mutex locks
- Each connection has a dedicated thread for receiving messages

### Thread Management
- All created threads are detached to avoid resource leaks
- Thread safety is ensured using mutex for critical sections
- Signal handling for clean program termination

### Message Handling
- Messages can be sent to both outgoing and incoming connections
- Real-time display of received messages
- Support for messages up to 100 characters in length

## Building and Running

### Building the Application
Use the provided Makefile to build the application:
```bash
make
```

### Running the Application
Run the application with a port number as an argument:
```bash
./bin/chat_app <port>
```
Example:
```bash
./bin/chat_app 8080
```

### Memory Leak Testing
To check for memory leaks using Valgrind:
```bash
make valgrind-check
```

To save the Valgrind output to a log file:
```bash
make valgrind-log
```

## Example Usage

### Starting the Application
```bash
$ ./bin/chat_app 8080
Listening on port: 8080
************************Command List*************************
help                         : Display all commands
myip                         : Display IP of this device
myport                       : Display port of this device
connect <ip> <port_num>      : Connect to device with IP and port
list                         : Display all connected devices
send <id> <message>          : Send message to device with id
terminate <id>               : Disconnect with device at id
exit                         : Close application
************************************************************
```

### Checking Your IP and Port
```bash
Enter command: myip
My IP is: 192.168.1.5
Enter command: myport
My port is: 8080
```

### Connecting to Another Device
```bash
Enter command: connect 192.168.1.10 9000
Connected to IP: 192.168.1.10, Port: 9000
```

### Listing All Connections
```bash
Enter command: list

****************** Device Connections *******************
ID |        IP Address         | Port No.
--------------------------------------------------------
0  | 192.168.1.10              | 9000
100| 192.168.1.15              | 45678 (incoming)
********************************************************
```

### Sending a Message
```bash
Enter command: send 0 Hello, how are you today?
Message sent to device with ID: 0
```

### Receiving a Message
```
***Message received from: 192.168.1.10
***Sender Port:          45678
-->Message:              Hi there! I'm doing well, thanks for asking.

Enter command: 
```

### Terminating a Connection
```bash
Enter command: terminate 0
Connection with ID 0 terminated successfully.
```

### Exiting the Application
```bash
Enter command: exit
Exiting application...
Cleaning up resources...
All resources cleaned up.
```

## Cleaning up
To remove all object files and executables:
```bash
make clean
```
