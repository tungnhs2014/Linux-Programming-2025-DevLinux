# Linux Process

##  3.1. Introduction
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
##  3.2. Command-line Arguments
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
##  3.3. Memory Layout
### 3.3.1. Memory Segments (From Low to High Addresses)
#### Text/Code Segment
- **Purpose**: Contains executable machine code instructions of the program
- **Permissions**: Read-only (prevents modification and allows sharing)
- **Location**: Lowest addresses (typically starting at 0x08048000 in 32-bit systems)
- **Characteristics**:
  - Shared among multiple processes running the same program
  - Fixed size determined at compile time
  - Marked with `_etext` symbol at the end
  - Loaded directly from the executable file's .text section

#### Initialized Data Segment (.data)
- **Purpose**: Stores global and static variables with explicit initialization
- **Permissions**: Read-write
- **Location**: Immediately above the text segment
- **Characteristics**:
  - Contains pre-initialized values specified in the source code
  - Fixed size determined at compile time
  - Loaded from the executable file's .data section
  - Marked with `_edata` symbol at the end
  - Example: `static int counter = 10;` or `char *message = "Hello";`

#### Uninitialized Data Segment (BSS - Block Started by Symbol)
- **Purpose**: Contains global and static variables without explicit initialization
- **Permissions**: Read-write
- **Location**: Above the initialized data segment
- **Characteristics**:
  - All memory is zeroed by the kernel at program start
  - Does not occupy space in the executable file (only size information)
  - Marked with `_end` symbol at the end
  - Example: `static char buffer[8192];` or `int array[1000];`

#### Heap
- **Purpose**: Area for dynamic memory allocation during runtime
- **Permissions**: Read-write
- **Location**: Starts immediately after the BSS segment
- **Characteristics**:
  - Grows upward (toward higher addresses)
  - Managed via malloc(), calloc(), realloc(), free() in C
  - Controlled by program break point, adjusted with brk() and sbrk() system calls
  - Not automatically freed when variables go out of scope
  - Can lead to memory fragmentation and leaks if not properly managed
  - Size can change during program execution

#### Memory Mapping Segment
- **Purpose**: Region where files are mapped into memory and shared libraries are loaded
- **Permissions**: Varies based on mapping type (read-only, read-write, or executable)
- **Location**: Between heap and stack, in the middle of the address space
- **Characteristics**:
  - Created by mmap() system call
  - Contains dynamically loaded libraries (e.g., libc.so)
  - Used for memory-mapped files
  - Anonymous mappings for thread stacks
  - Shared memory regions between processes
  - Can be shared or private

#### Stack
- **Purpose**: Supports function call mechanism and stores local variables
- **Permissions**: Read-write
- **Location**: High memory addresses (just below kernel space in 32-bit systems)
- **Characteristics**:
  - Grows downward (toward lower addresses)
  - LIFO (Last In, First Out) structure
  - Automatically managed during function calls and returns
  - Contains:
    - Function parameters
    - Return addresses
    - Local variables
    - Saved register values
  - Limited in size (typically 8MB by default, configurable with ulimit)
  - Stack frame created on function entry, destroyed on function exit
  - Stack overflow occurs if the limit is exceeded

#### Kernel Space
- **Purpose**: Reserved for kernel code, data structures, and device drivers
- **Permissions**: Not accessible from user mode (protection ring)
- **Location**: Highest addresses (above 0xC0000000 in 32-bit systems)
- **Characteristics**:
  - Mapped into every process's address space
  - Protected by CPU privilege levels
  - Accessed indirectly through system calls
  - Contains kernel-specific data like process tables, file system cache
  - In 32-bit systems, typically occupies the top 1GB of the 4GB virtual address space

### 3.3.2. Memory Address Layout

#### 32-bit Linux Systems
```
0xFFFFFFFF  +----------------------+
            |                      |
            |    Kernel Space      |  (~1GB)
            |                      |
0xC0000000  +----------------------+
            |         Stack        |  (grows downward ↓)
            +----------------------+
            |                      |
            | Memory Mapping       |
            | Segment              |
            |                      |
            +----------------------+
            |                      |
            |    Unused Memory     |
            |                      |
            +----------------------+
            |         Heap         |  (grows upward ↑)
            +----------------------+
            |         BSS          |
            +----------------------+
            |    Initialized Data  |
            +----------------------+
            |         Text         |
            +----------------------+
0x08048000  |                      |
            | Reserved/Unmapped    |
            |                      |
0x00000000  +----------------------+
```

### 3.3.3. Security Mechanisms
- **ASLR (Address Space Layout Randomization)**: Randomizes base addresses of segments to prevent memory exploitation attacks
- **Stack canaries**: Special values placed before return addresses to detect stack overflows
- **NX bit (No-Execute)**: Marks data pages as non-executable
- **Position Independent Executables (PIE)**: Allow code to be loaded at any address
- **Memory page permissions**: Granular control over read/write/execute permissions

### 3.3.4. Common Memory Issues
- **Segmentation Fault**: Occurs when a program tries to access memory it doesn't have permission to access
- **Stack Overflow**: Happens when the stack exceeds its size limit (often due to infinite recursion)
- **Memory Leak**: Results from allocating memory that is never freed
- **Buffer Overflow**: Occurs when writing beyond the bounds of allocated memory
- **Use After Free**: Using memory after it has been deallocated
- **Memory Fragmentation**: Occurs when free memory is broken into small, non-contiguous chunks

### 3.3.5. System Commands for Memory Inspection
- `cat /proc/<pid>/maps`: View memory mappings of a specific process
- `pmap <pid>`: Display memory map of a process (similar to above but formatted)
- `size <executable>`: Show the size of text, data, and BSS segments
- `ulimit -s`: Check or set the stack size limit
- `free`: Display system memory usage
- `top` or `htop`: Monitor process memory usage in real-time
- `valgrind`: Tool for memory debugging, leak detection, and profiling

### 3.3.6. Process Memory Creation Lifecycle
1. The kernel loads the program executable
2. Text and data segments are created based on the executable file
3. BSS segment is created and zeroed
4. Heap is initialized
5. Stack is created with initial environment and arguments
6. Shared libraries are mapped into memory
7. Control is transferred to the program entry point (typically `_start` then `main()`)

---

##  3.4. Operations on Process
### 3.4.1. System call Fork()
The `fork()` system call is a fundamental mechanism for process creation in Linux and Unix-based operating systems. When a process calls `fork()`, the operating system creates a new process that is almost identical to the calling process.

![Image](https://github.com/user-attachments/assets/b4810c85-6346-46d4-9a7d-de96e5e66cba)

#### Basic Concepts
- **Parent Process**: The process that calls the `fork()` system call
- **Child Process**: The new process created by the parent process
- **Init Process**: The first process in the system (PID 1), which is the parent of all other processes

#### How It Works
- When a process calls `fork()`, the following occurs:
  + The kernel creates a new process (the child)
  + The child receives a copy of the parent's data, heap, and stack segments
  + Both processes continue execution from the instruction following the `fork()` call

#### Return Values
- The `fork()` system call returns different values depending on whether it's the parent or child process:
  + In the parent process: Returns the child's Process ID (PID)
  + In the child process: Returns 0
  + On failure: Returns -1

#### Memory Behavior
- After a fork operation: 
  + Each process has its own text segment
  + The child receives a copy of the parent's data, heap, and stack
  + Changes to variables in one process do not affect the other process

#### Example

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    int a = 0;
    pid_t pid;
    
    printf("Initial value: a = %d\n", a);
    
    pid = fork();
    
    if (pid < 0) {
        // Error handling
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        a++;
        printf("Child process: a = %d\n", a);
    } else {
        // Parent process
        printf("Parent process: a = %d, Child PID = %d\n", a, pid);
    }
    
    return 0;
}
```

#### Common Uses
- Creating parallel processes for concurrent tasks
- Implementing command shells
- Creating daemon processes
- Executing different programs (when combined with exec functions)

### 3.4.2 Exec Family - Process Replacement Functions
The Exec Family is a group of system calls in Linux used in conjunction with fork(). While fork() creates a child process that is identical to the parent, the Exec Family allows replacing the current process with a new program.

#### How It Works
- Exec functions replace the code, data, heap, and stack of the calling process with a new program
- The process retains its PID after the exec call
- Typically used after fork() to run a different program in the child process

#### Functions in the Exec Family
1. **execle(const char \*pathname, const char \*arg, ..., char \*const envp[])**
   - Executes a program with a full pathname
   - Passes arguments as a list
   - Allows setting environment variables

2. **execlp(const char \*filename, const char \*arg, ...)**
   - Searches for filename in PATH directories
   - Passes arguments as a list

3. **execvp(const char \*filename, char \*const argv[])**
   - Searches for filename in PATH directories
   - Passes arguments as a vector array

4. **execv(const char \*pathname, char \*const argv[])**
   - Executes a program with a full pathname
   - Passes arguments as a vector array

5. **execl(const char \*pathname, const char \*arg, ...)**
   - Executes a program with a full pathname
   - Passes arguments as a list

#### Key Characteristics
- None of the exec functions return on success
- Return -1 on error
- Work with fork() to create the "fork and exec" model
- The process after exec inherits PID, PPID, and many attributes from the original process

#### The "Fork and Exec" Model
- This model is the foundation of many Linux applications such as shells, daemons, and process management utilities:
  + Parent process calls fork() to create a child process
  + Child process calls one of the exec functions to run a new program
  + Parent process can continue execution or wait for the child process to terminate

#### Example Code
This example demonstrates the "fork and exec" pattern by creating a child process that executes the `ls -l` command while the parent waits for it to complete.
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    
    pid = fork();
    
    if (pid < 0) {
        // Error
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("Child process executing ls command...\n");
        
        // Replace child process with the ls command
        execl("/bin/ls", "ls", "-l", NULL);
        
        // If execl returns, it must have failed
        fprintf(stderr, "execl failed\n");
        exit(1);
    } else {
        // Parent process
        printf("Parent waiting for child (PID: %d) to complete...\n", pid);
        wait(NULL);
        printf("Child process completed\n");
    }
    
    return 0;
}
```
--- 

## 3.5. Process Management
Process Management is performed through a number of basic system calls. These system calls can be combined to solve more complex problems. Process management is mainly handled through **system calls**, including:
- `fork()`: Creates a new process.
- `exit()`: Terminates a process and returns an exit status.
- `wait()`: Waits for a process to terminate and collects its exit status.
- `waitpid()`: Waits for a specific child process to terminate.

![Image](https://github.com/user-attachments/assets/368ca4a6-2058-4e9a-b0c9-c1cbb0588ac4)

### System Calls
#### fork()
- Creates a new process (child) from the calling process (parent).
- Returns:
  - `0` to the child process.
  - Child's **PID** to the parent process.
  - `-1` if process creation failed.

#### exit()
- Terminates the calling process.
- Returns an **exit status** to the parent process.
- Ensures that the resources are properly released.

#### wait()
- Used to monitor the termination status of one of the child processes created by the parent process.
- Blocks the parent process until a child process terminates.  
  - If a child process has already terminated, it returns immediately.
- If the `status` is other than **-1**, it points to an integer value containing information about the child's termination state.
- Returns the **PID of the terminated child process** or **-1** if an error occurs.
```c
#include <sys/wait.h>


pid_t wait(int *status);
```

#### waitpid()
- Addresses the limitation of `wait()` when there are multiple child processes.  
- Allows the parent process to wait for a **specific child process** to terminate.  
- Can be set to **non-blocking mode** using the `WNOHANG` option.  
- Returns the **PID of the terminated child process** or **-1** if an error occurs.

#### Syntax
```c
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);
```
- `pid`:
  - `> 0`: PID of the specific child process to wait for.
  - `= 0`: Wait for any child process in the same process group.
  - `< -1`: Wait for any child process whose process group ID is the absolute value of `pid`.
  - `= -1`: Wait for any child process (similar to `wait()`).
- `status`: Points to an integer where the termination status will be stored.
- `options`: Additional options to modify behavior (e.g., `WNOHANG` for non-blocking wait).

---

### Example Code Wait()

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    if (pid == 0) {
        // Child process
        printf("Child process (PID: %d) is running\n", getpid());
        exit(0);
    } else {
        // Parent process
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Parent process (PID: %d) received exit status: %d\n", getpid(), WEXITSTATUS(status));
        }
    }

    return 0;
}
```

### Example Code Waitpid()

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        printf("Child process (PID: %d) is running\n", getpid());
        sleep(2);  // Simulate some work
        exit(42);  // Exit with a specific code
    } else {
        // Parent process
        int status;
        pid_t child_pid = waitpid(pid, &status, 0);
        if (child_pid > 0) {
            if (WIFEXITED(status)) {
                printf("Parent process (PID: %d) received exit status from child (PID: %d): %d\n",
                       getpid(), child_pid, WEXITSTATUS(status));
            }
        } else {
            perror("waitpid failed");
        }
    }

    return 0;
}
```
--- 

## 3.6. Orphan and Zombie Processes
### 3.6.1. Orphan Process
- An **orphan process** occurs when the **parent process terminates before the child process**.
- The **orphaned child process** is adopted by the **`init` process (PID 1)**.
- The `init` process will perform a `wait()`to clean up the process and release resources.

#### Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid > 0) {
        // Parent process
        printf("Parent process (PID: %d) exiting...\n", getpid());
        exit(0);
    } else if (pid == 0) {
        // Child process
        sleep(5);  // Simulate a long-running child process
        printf("Orphan child process (PID: %d), adopted by init (PID: 1)\n", getpid());
        exit(0);
    } else {
        perror("Fork failed");
        exit(1);
    }

    return 0;
}
```
**Explanation**:
- The parent process terminates immediately after forking.
- The child process continues to run and becomes an orphan.
- The init process adopts the orphaned child and reaps it when it finishes.

### 3.6.2. Zombie Process
- A **zombie process** occurs when the child process terminates before the parent process but the parent does not call `wait()` or `waitpid()`.
- The **child process becomes a zombie** and remains in the process **table as "Z" (Zombie)**.
- This occurs because the exit status of the child is not collected by the parent.

#### Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid > 0) {
        // Parent process: Sleep to simulate delay
        sleep(10);
        printf("Parent process (PID: %d) running, child became zombie.\n", getpid());
    } else if (pid == 0) {
        // Child process: Exit immediately
        printf("Child process (PID: %d) exiting...\n", getpid());
        exit(0);
    } else {
        perror("Fork failed");
        exit(1);
    }

    return 0;
}
```

#### How to Identify
Use the following command to detect zombie processes:
```bash
ps aux | grep Z
```

### 3.6.3. Prevent Zombie 
- Too many zombie processes can **fill the Process ID (PID) table**, preventing the creation of new processes.
- Two primary methods to prevent zombie processes:
  + Using `wait()` or `waitpid()` to collect the child's termination status.
  + Using signal handling **(SIGCHLD)** to automatically reap child processes.

#### Example 1: Using waitpid()
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid > 0) {
        // Parent process: Wait for the child process to terminate
        waitpid(pid, NULL, 0);
        printf("Child process reaped by parent.\n");
    } else if (pid == 0) {
        // Child process
        printf("Child process (PID: %d) exiting...\n", getpid());
        exit(0);
    } else {
        perror("Fork failed");
        exit(1);
    }

    return 0;
}
```

#### Example 2: Using SIGCHLD Handler
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void sigchld_handler(int signum) {
    wait(NULL);  // Reap the child process
    printf("Child process reaped automatically.\n");
}

int main() {
    signal(SIGCHLD, sigchld_handler);  // Set up signal handler

    pid_t pid = fork();

    if (pid > 0) {
        // Parent process
        printf("Parent process (PID: %d) running...\n", getpid());
        sleep(10);  // Simulate long-running parent
    } else if (pid == 0) {
        // Child process
        printf("Child process (PID: %d) exiting...\n", getpid());
        exit(0);
    } else {
        perror("Fork failed");
        exit(1);
    }

    return 0;
}
```

### Summary
- **Orphan Process:** Child continues running after the parent exits and is adopted by init.
- **Zombie Process:** Child terminates but the parent does not wait(), leaving it in the process table.
- **Preventing Zombie Process:**
  + Use wait() or waitpid() in the parent process.
  + Set up a SIGCHLD signal handler to automatically reap terminated children.
- By managing orphan and zombie processes effectively, you ensure the stability and efficiency of Linux systems