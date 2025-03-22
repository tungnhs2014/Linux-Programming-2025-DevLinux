# IPC Signals

## 4.1. Introduction and Signal Fundamentals
Signals are one of the oldest inter-process communication (IPC) mechanisms in Unix/Linux systems. Essentially, a signal is a software interrupt used to handle asynchronous events.

### 4.1.1. Signal Characteristics:
- Lightweight and simple
- Carry minimal data (only signal number)
- Can be sent between unrelated processes
- Suitable for simple event notifications

### 4.1.2. Signal Origins:
1. **From hardware**: When hardware errors occur (e.g., segmentation fault)
2. **From users**: When users press key combinations (e.g., Ctrl+C)
3. **From kernel**: When system events occur (e.g., child process termination)
4. **From other processes**: When a process sends a signal to another process

### 4.1.3. Signal Lifecycle

![Signal Lifecycle](https://github.com/user-attachments/assets/1bd2b086-3763-4688-b4cf-8544425a3e00)

**Each signal goes through three main phases:**
1. **Generation**: Signal is created due to some event
2. **Delivery**: Signal remains in pending state until delivered
3. **Processing**: When the signal is delivered, it's processed in one of three ways:

#### Signal Processing Methods:

- **Ignore the signal**: No action taken, process continues execution normally (SIGKILL and SIGSTOP cannot be ignored)
- **Catch and handle the signal**: Kernel temporarily suspends the main thread and transfers control to the signal handler (SIGKILL and SIGSTOP cannot be caught)
- **Perform the default action**: Each signal has its own default action (terminate, stop, continue, or ignore)

### 4.1.4. Basic Signals

| Signal   | No. | Description                                    | Default Action | Catchable? | Ignorable? |
|----------|----:|------------------------------------------------|----------------|------------|------------|
| SIGKILL  | 9   | Terminate process immediately                  | Terminate      | No         | No         |
| SIGTERM  | 15  | Terminate process (allows cleanup)             | Terminate      | Yes        | Yes        |
| SIGINT   | 2   | Keyboard interrupt (Ctrl+C)                    | Terminate      | Yes        | Yes        |
| SIGCHLD  | 17  | Child process terminated/stopped               | Ignore         | Yes        | Yes        |
| SIGSTOP  | 19  | Stop process                                   | Stop           | No         | No         |
| SIGCONT  | 18  | Continue process (if stopped)                  | Continue       | Yes        | Yes        |
| SIGUSR1  | 10  | User-defined signal                            | Terminate      | Yes        | Yes        |
| SIGUSR2  | 12  | User-defined signal                            | Terminate      | Yes        | Yes        |
| SIGSEGV  | 11  | Invalid memory access                          | Terminate+Core | Yes        | Yes        |
| SIGHUP   | 1   | Terminal closed, often used to reload config   | Terminate      | Yes        | Yes        |

### 4.1.5. Special Signals:
- **SIGKILL (9)**: Cannot be ignored or caught, always terminates the process
- **SIGSTOP (19)**: Cannot be ignored or caught, always stops the process
- **SIGUSR1 (10) and SIGUSR2 (12)**: Reserved for applications to define their own usage

--- 

## 4.2. Signal Handling and Implementation
### 4.2.1 Signal Handler
A signal handler is a function called when a specific signal is sent to a process. To register a signal handler, we use the `signal()` or `sigaction()` system call.

### 4.2.2. Registering a Signal Handler with signal()

```c
sighandler_t signal(int signo, sighandler_t handler);
```

Parameters:
- `signo`: Signal number to handle
- `handler`: Function pointer to the signal handler

Return value:
- If successful: Previous value of the signal handler
- If failed: SIG_ERR

### 4.2.3. Simple Example:

```c
#include <stdio.h>
#include <signal.h>

void handle_sigint(int sig) {
    printf("SIGINT signal received!\n");
}

int main() {
    signal(SIGINT, handle_sigint);
    
    printf("Press Ctrl+C to send SIGINT...\n");
    
    while(1) {
        // Infinite loop
    }
    
    return 0;
}
```

### 4.2.4. Registering a Signal Handler with sigaction() (Recommended)

```c
int sigaction(int signo, const struct sigaction *act, struct sigaction *oldact);
```

`sigaction` provides more options and control than `signal()`:

```c
struct sigaction {
    void     (*sa_handler)(int);
    void     (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t   sa_mask;
    int        sa_flags;
};
```

### 4.2.5. Notes when writing Signal Handlers:
- Signal handlers should be short and simple
- Avoid using non-async-signal-safe functions
- Don't allocate/free memory in handlers
- Don't use global variables without synchronization mechanisms
- Return quickly from handlers

---

## 4.3. Signal Management and Control
### 4.3.1. Sending Signals

There are multiple ways to send signals to a process:

#### Using the kill() Function

```c
int kill(pid_t pid, int signo);
```

Parameters:
- `pid`: Target process ID
- `signo`: Signal number to send

Special values for `pid`:
- `pid > 0`: Send signal to the specific process with PID
- `pid == 0`: Send signal to all processes in the same process group
- `pid == -1`: Send signal to all processes that the sender has permission to
- `pid < -1`: Send signal to all processes in the group with ID = |pid|

#### Using the kill Command in Terminal

```bash
kill -SIGNAL PID
```

Examples:
```bash
kill -9 1234       # Send SIGKILL to process 1234
kill -TERM 5678    # Send SIGTERM to process 5678
```

#### Sending a Signal to the Current Process

```c
kill(getpid(), SIGUSR1);
```

#### Signal Keyboard Shortcuts

| Shortcut | Signal  | Description                     |
|----------|---------|----------------------------------|
| Ctrl+C   | SIGINT  | Interrupt foreground process     |
| Ctrl+Z   | SIGTSTP | Suspend foreground process       |
| Ctrl+\   | SIGQUIT | Terminate and create core dump   |

### 4.3.2. Blocking and Unblocking Signals

#### Why Block Signals?
- Signals can interrupt a process at any time
- Sometimes critical code sections must not be interrupted
- Blocking signals helps control when signals are processed

#### Signal Sets
Signal sets are data structures to represent sets of signals:

```c
typedef struct {
    unsigned long sig[_NSIG_WORDS];
} sigset_t;
```

Functions to manipulate signal sets:

```c
int sigemptyset(sigset_t *set);           // Initialize empty set
int sigfillset(sigset_t *set);            // Initialize with all signals
int sigaddset(sigset_t *set, int signo);  // Add signal to set
int sigdelset(sigset_t *set, int signo);  // Remove signal from set
int sigismember(const sigset_t *set, int signo);  // Check if signal is in set
```

#### Changing Signal Mask with sigprocmask()

```c
int sigprocmask(int how, const sigset_t *newset, sigset_t *oldset);
```

Parameters:
- `how`: Specifies the type of change
  - `SIG_SETMASK`: Set signal mask to `newset`
  - `SIG_BLOCK`: Add `newset` to current signal mask (OR operation)
  - `SIG_UNBLOCK`: Remove `newset` from current signal mask
- `newset`: New signal set to apply (or NULL)
- `oldset`: Location to store previous signal mask (or NULL)

#### Example: Temporarily Blocking SIGINT

```c
void critical_section(void) {
    sigset_t new_mask, old_mask;
    
    // Initialize empty set and add SIGINT
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    
    // Block SIGINT and save old mask
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
    
    printf("In critical section, SIGINT is blocked\n");
    sleep(5);  // Simulate important code
    
    // Restore original signal mask
    sigprocmask(SIG_SETMASK, &old_mask, NULL);
    printf("Left critical section, SIGINT is unblocked\n");
}
```

#### Checking Pending Signals

```c
int sigpending(sigset_t *set);
```

This function stores the set of pending signals in `set`.

---

## 4.4. Practical Examples

### 4.4.1.Example 1: Simple Signal Handling

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_signal(int signo) {
    if (signo == SIGINT)
        printf("\nReceived SIGINT. Press Ctrl+C again to exit.\n");
    else if (signo == SIGTERM)
        printf("\nReceived SIGTERM. Cleaning up and exiting.\n");
    else
        printf("\nReceived signal: %d\n", signo);
}

int main(void) {
    // Register signal handlers
    if (signal(SIGINT, handle_signal) == SIG_ERR)
        printf("Cannot handle SIGINT\n");
    
    if (signal(SIGTERM, handle_signal) == SIG_ERR)
        printf("Cannot handle SIGTERM\n");
    
    // Infinite loop
    printf("Process PID: %d\n", getpid());
    printf("Press Ctrl+C or use 'kill -TERM %d' to test\n", getpid());
    
    while(1) {
        printf("Process running...\n");
        sleep(3);
    }
    
    return 0;
}
```

### 4.4.2. Example 2: Using sigaction() Instead of signal()

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void handle_signal(int signo) {
    printf("\nCaught signal %d\n", signo);
}

int main(void) {
    struct sigaction sa;
    
    // Clear sigaction structure
    memset(&sa, 0, sizeof(sa));
    
    // Set handler
    sa.sa_handler = handle_signal;
    
    // Block other signals during handling
    sigfillset(&sa.sa_mask);
    
    // Set flags (e.g., SA_RESTART to automatically restart interrupted system calls)
    sa.sa_flags = SA_RESTART;
    
    // Register handler for SIGINT
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    
    printf("Process PID: %d\n", getpid());
    printf("Press Ctrl+C to test handler\n");
    
    // Infinite loop
    while(1) {
        printf("Process running...\n");
        sleep(3);
    }
    
    return 0;
}
```

### 4.4.3. Example 3: Sending Signals to Another Process

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <pid> <signal_number>\n", argv[0]);
        return 1;
    }
    
    pid_t pid = atoi(argv[1]);
    int signo = atoi(argv[2]);
    
    printf("Sending signal %d to process %d\n", signo, pid);
    
    if (kill(pid, signo) == -1) {
        perror("kill");
        return 1;
    }
    
    printf("Signal sent successfully\n");
    return 0;
}
```

### 4.4.4. Example 4: Using Signal Masks to Protect Critical Sections

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int signo) {
    printf("\nSIGINT has been caught!\n");
}

void critical_section(void) {
    sigset_t new_mask, old_mask;
    
    printf("Starting critical section\n");
    
    // Initialize new signal set and add SIGINT
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    
    // Block SIGINT and save old mask
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask");
        exit(1);
    }
    
    printf("SIGINT is blocked. Try pressing Ctrl+C...\n");
    sleep(5);  // Simulate important work
    
    // Check if any signals are pending
    sigset_t pending;
    if (sigpending(&pending) < 0) {
        perror("sigpending");
        exit(1);
    }
    
    if (sigismember(&pending, SIGINT))
        printf("SIGINT is pending!\n");
    
    // Restore original signal mask
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask");
        exit(1);
    }
    
    printf("SIGINT is unblocked. Critical section ended.\n");
}

int main(void) {
    // Register handler for SIGINT
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    
    printf("Press Enter to start critical section...");
    getchar();
    
    critical_section();
    
    printf("Process continues running. Press Ctrl+C to exit.\n");
    while(1) {
        sleep(1);
    }
    
    return 0;
}
```
--- 

## 4.5. Best Practices and Summary
### 4.5.1.Best Practices
1. **Use sigaction() Instead of signal()**
   - `sigaction()` provides more control and consistent behavior
   - `signal()` may have different behavior on different Unix systems

2. **Keep Signal Handlers Simple**
   - Signal handlers should execute quickly
   - Avoid complex operations, system calls, or memory allocation
   - Better to set a flag and return immediately, handling the main logic outside the handler

3. **Be Careful with Signal Safety**
   - Not all functions are safe to call from signal handlers
   - Only use "async-signal-safe" functions in handlers
   - Refer to system documentation for a list of safe functions

4. **Block Signals in Critical Sections**
   - Use `sigprocmask()` to temporarily block signals during critical operations
   - Ensure signal masks are restored afterward

5. **Handle Interrupted System Calls**
   - Many system calls can be interrupted by signals
   - Check for EINTR error code and restart system calls if needed
   - Use the SA_RESTART flag in sigaction() for automatic restarting

6. **Always Check Return Values**
   - Signal-related functions can fail
   - Always check return values and handle errors appropriately

7. **Be Cautious with Race Conditions**
   - Signals create asynchronous execution that can lead to race conditions
   - Use appropriate synchronization mechanisms when accessing shared resources

### 4.5.2. Summary
Signals are an important mechanism in Linux system programming, allowing communication between processes and handling asynchronous events. Although simple, signals are powerful and flexible.

#### Advantages of Signals:
- Simple, lightweight mechanism
- Widely supported on all Unix/Linux systems
- Suitable for simple event notifications

#### Limitations of Signals:
- Cannot carry much data
- Can be lost if the same type of signal occurs multiple times while pending
- Requires careful attention to synchronization issues

#### When to Use Signals:
- Handling asynchronous events (like Ctrl+C)
- Managing process lifecycle (parent-child)
- Simple event notifications between processes
- Handling timeouts and alarms

#### When Not to Use Signals:
- Transferring complex data between processes
- Detailed synchronization between processes
- High-frequency communication between processes

For more complex communication, consider other IPC mechanisms such as pipes, message queues, shared memory, or sockets.

---