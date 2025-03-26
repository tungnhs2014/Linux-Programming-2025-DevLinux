# Exercise 5: Sharing Resources with Read-Write Lock

## Description
Write a program to simulate a scenario where multiple threads read and write shared data:

1. Create a shared integer variable `data`.
2. Create 5 reader threads and 2 writer threads:
   - **Reader threads** only read `data` without modifying it.
   - **Writer threads** increase the value of data by 1.
3. Use a read-write lock to ensure:
   - Multiple reader threads can read simultaneously.
   - Only one writer thread can modify `data` at a time.
4. Print the final value of data after all threads complete execution.


## Hint
- Use `pthread_rwlock_rdlock` for reading and `pthread_rwlock_wrlock` for writing to maintain proper synchronization.

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