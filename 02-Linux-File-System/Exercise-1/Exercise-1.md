## Exercise 1: 

- Write a program that opens any file using the O_APPEND flag. Then, perform a seek operation to move to the beginning of the file and write some data to it. Where will the data appear in the file, and why does this happen?

## Answer
- The data will always be written at the end of the file, regardless of the `lseek(fd, 0, SEEK_SET)` call.

## Explanation
- The file is opened with the `O_APPEND` flag, which forces all write operations to append data at the end of the file.
- Even if the program explicitly calls `lseek(fd, 0, SEEK_SET)` to move the file pointer to the beginning, this does not affect the writing position.
- When `write(fd, buf, strlen(buf))` is executed, the system ensures that data is always appended to the end of the file.

## Project Structure
The project follows this directory structure:
```
.
├── Makefile
├── src
│   ├── main.c
├── obj
├── bin
└── README.md
```
- `src/` contains the source code (`.c` files).
- `obj/` stores compiled object files (`.o` files).
- `bin/` contains the final executable.
- `Makefile` defines the build rules.

## How to Build and Run
### Build the project:
```sh
make
```
This compiles the source code and generates the executable in `bin/exam`.

### Run the program:
```sh
./bin/exam
```

### Clean the build files:
```sh
make clean
```
This removes all compiled files and directories (`obj/`, `bin/`).

