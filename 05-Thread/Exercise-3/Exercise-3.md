# Exercise 3: Using Condition Variables

## Description
Write a program with two threads: producer and consumer, as follows:

## Instructions
1. **Producer:** Generates a random number between 1 and 10 and stores it in a global variable called `data`.
2. **Consumer:** Waits for the producer to provide data, then reads and prints the data.
3. Use ``pthread_cond_wait` and `pthread_cond_signal` to synchronize between producer and consumer, ensuring the consumer reads the data only after the producer has generated it.
4. Repeat the above process **10 times** and print all the values read by the consumer.

## Hint
- Use ``pthread_cond_wait` to make the consumer wait until it receives a signal from the producer indicating that the data is ready.

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

### Example output
```bash
```

### Clean the project
To remove all compiled files and binaries, run:
```bash
$ make clean
```