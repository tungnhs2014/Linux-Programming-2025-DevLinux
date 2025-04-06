# Exercise 1: Simple Message Sending and Receiving with POSIX Message Queue

## Description
Write a program that uses **POSIX message queues** to send and receive a simple message

## Requirements
1. Create a message queue using the `mq_open()` function.
2. The **parent process** sends a string message to the **child process** through the message queue.
3. The **child process** receives the message and prints its content.

## Hint
- Use the `mq_attr` structure to define the message attributes (like size and max messages).
- Use `mq_send()` and `mq_receive()` to send and receive messages.
- Don't forget to close and `unlink` the message queue after usage to avoid resource leaks.

---

## Project Structure
```
project/
├── bin/         
│   └── exam
├── Makefile   
├── obj/         
│   └── main.o
└── src/         
    └── main.c
```

---

## Build and Run Instructions

### Build the program
To build the program, run:
```bash
$ make
```

### Run the program
To execute the program, use:
```bash
$ ./bin/exam
```

### Example Run
```bash

```

### Clean the project
To remove all compiled files and binaries, run:
```bash
$ make clean
```
