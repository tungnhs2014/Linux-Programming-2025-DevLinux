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

## 🔧 3.4. Operations on Process
## 🗃️ 3.5. Process Management
## 👻 3.6. Orphan and Zombie Processes
## 📝 3.7. Assignments
