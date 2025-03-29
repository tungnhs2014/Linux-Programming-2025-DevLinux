# Exercise 6: Summing an Array with Threads and Mutex

## Description
Write a program to compute the sum of a large array containing 1 million integers using multithreading and mutex for synchronization.

## Step
1. **Divide the array** into **4 equal parts**.
2. **Create 4 threads**, each responsible for summing **one part of the array**.
3. Use a **global sum variable** and **mutex** to safely aggregate results from all threads.
4. Print the **final sum** after all threads finish execution.


## Hint
- Use `pthread_mutex_lock` and `pthread_mutex_unlock` to protect the shared sum variable when threads update it.

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
Initializing array with 1 million random numbers...
Creating 4 threads to calculate sum...
Sum of the array: 49486907
```

### Clean the project
To remove all compiled files and binaries, run:
```bash
$ make clean
```