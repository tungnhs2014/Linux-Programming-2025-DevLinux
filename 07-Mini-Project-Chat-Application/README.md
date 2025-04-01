# Chat Application

## Overview
This is a command-line chat application that allows users to establish peer-to-peer connections between devices for real-time messaging. The application provides a simple interface for managing connections and exchanging messages with multiple connected users simultaneously.

## Features
- Peer-to-peer communication between devices
- Support for multiple simultaneous connections
- Real-time message exchange
- Connection management (establish, list, terminate)
- Thread-based concurrent processing
- Simple command interface

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
│   ├── chat.o          
│   ├── command.o       
│   ├── connection.o    
│   ├── message.o      
│   └── utils.o         
├── README.md           
└── src                 # Source code
    ├── chat.c          # Main application code
    ├── command.c       # Command implementation
    ├── connection.c    # Connection handling implementation
    ├── message.c       # Message processing implementation
    └── utils.c         # Utility functions implementation
```

## Command List
The application supports the following commands:
- `help`: Display all available commands
- `myip`: Display IP of this device
- `myport`: Display port of this device
- `connect <ip> <port_num>`: Connect to device with specified IP and port
- `list`: Display all connected devices
- `send <id> <message>`: Send message to device with id
- `terminate <id>`: Disconnect from device at id
- `exit`: Close application

## Implementation Details
The application uses:
- POSIX threads for handling incoming connections and messages
- Socket programming for network communications
- A command parser for processing user input
- Signal handling for graceful termination

## Building and Running
### Building the Application
Use the provided Makefile to build the application:
```bash
make
```

This will compile the source files and generate the executable in the `bin` directory.

### Running the Application
Run the application with a port number:

```bash
./bin/chat_app <port> (Ex: ./bin/chat_app 8080)
```

### Memory Leak Checking
Valgrind can be used to check for memory leaks:
```bash
maek valgrind-check
```
Valgrind can be used to check for memory leaks and save log:
```bash
maek valgrind-log
```

## Example Session
```bash
$ ./bin/chat_app 8080
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
Enter command: myip
IP: 192.168.233.128
Enter command: connect 192.168.233.128 9000
Connected to IP: 192.168.233.128, Port: 9000
Enter command: list
Device connection
ID |        IP Address         | Port No.
ID: 0 | IP: 192.168.233.128 | Port: 9000
*****************************************
Enter command: send 1 Hello, how are you?
Message Hello how are you?
```

## Cleaning up
To clean the project (remove object files and executables):

```bash
make clean
```