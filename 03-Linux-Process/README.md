# Linux Process

## 🚀 3.1. Introduction
### 3.1.1. Program
- A program is a set of instructions designed to perform a specific task
- Programs exist as executable files stored on the computer's hard drive
- A program is a static entity - just a collection of code stored in non-volatile memory
- Programs don't use system resources until executed

### 3.1.2. Process
- A process is a program in execution state, using system resources
- Each process has a unique identifier called Process ID (PID)
- PID is a positive and unique integer for each process on the system
- A process is a dynamic entity - it exists in memory and uses CPU, RAM and other resources
- One program can create multiple different processes when executed

### 3.1.3. Relationship Between Programs and Processes
- A process is created when a program is loaded into memory and begins execution
- Multiple processes can be created from the same program
- When a process completes its task, it terminates and releases system resources

---
## 📝 3.2. Command-line Arguments
### 3.2.1. Overview
Every program starts from the `main()` function. When running a program, the environment parameters (command line arguments) will be passed as two arguments in the main() function:

### 3.2.2. Main Function Parameters
#### argc
- Number of parameters passed to main() function
- Always at least 1 (program name counts as the first parameter)
- Used to determine how many arguments were provided
#### argv[]
- Array of pointers that point to the parameters passed to that program
- argv[0] contains the program name/path
- argv[1] through argv[argc-1] contain the actual command-line arguments

### 3.2.3 Example Usage
```c
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Number of arguments: %d\n", argc);
    
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    
    return 0;
}
```
When running: `./program hello world`
- argc will be 3
- argv[0] will be "./program"
- argv[1] will be "hello"
- argv[2] will be "world"

---
## 🧠 3.3. Memory Layout
## 🔧 3.4. Operations on Process
## 🗃️ 3.5. Process Management
## 👻 3.6. Orphan and Zombie Processes
## 📝 3.7. Assignments
