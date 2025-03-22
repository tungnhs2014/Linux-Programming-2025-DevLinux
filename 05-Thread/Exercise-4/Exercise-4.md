# Exercise 4: Counting Even and Odd Numbers with Threads

## Description
Write a program to perform the following tasks:
1. Generate a random array of **100 integers** with values ranging from **1 to 100**.
2. Create two threads:
   - One thread counts the **even numbers** in the array.
   - The other thread counts the **odd numbers** in the array.
3. Once both threads complete counting, print the total number of **even numbers** and **odd numbers**.
4. Use **pthread_join** to ensure the program only terminates when both threads finish their tasks.

## Hint
- Use **pthread_join** to ensure the program only terminates when both threads finish their tasks.

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
Total even numbers: 53
Total count numbers: 47
Total numers: 100
```

### Clean the project
To remove all compiled files and binaries, run:
```bash
$ make clean
```