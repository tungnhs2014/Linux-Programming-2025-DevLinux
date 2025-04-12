# 4.IPC Signals

## 4.1. Introduction and Signal Fundamentals
Signals are one of the oldest inter-process communication (IPC) mechanisms in Unix/Linux systems. Essentially, a signal is a software interrupt used to handle asynchronous events.

### 4.1.1. Signal Characteristics:
- Lightweight and simple
- Carry minimal data (only signal number)
- Can be sent between unrelated processes
- Suitable for simple event notifications

### 4.1.2. Signal Origins:

<p align="center">
  <img src="https://github.com/user-attachments/assets/1bd2b086-3763-4688-b4cf-8544425a3e00" width="70%" alt="Signal Origins">
</p>

Signals come from four main sources:

1. **From hardware**: When hardware errors occur (e.g., segmentation fault)
2. **From users**: When users press key combinations (e.g., Ctrl+C)
3. **From kernel**: When system events occur (e.g., child process termination)
4. **From other processes**: When a process sends a signal to another process

### 4.1.3. Signal Lifecycle

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

The following example shows how to handle multiple signals:

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Flag to control program exit
volatile sig_atomic_t keep_running = 1;

void signal_handler(int signo) {
    switch (signo) {
        case SIGINT:
            printf("\nReceived SIGINT (Ctrl+C). Press again to exit.\n");
            // Change handler to default action on next SIGINT
            signal(SIGINT, SIG_DFL);
            break;
            
        case SIGTERM:
            printf("\nReceived SIGTERM. Cleaning up and exiting.\n");
            keep_running = 0;
            break;
            
        case SIGUSR1:
            printf("\nReceived SIGUSR1. Performing custom action.\n");
            break;
    }
}

int main() {
    // Register signal handlers
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Failed to set SIGINT handler");
        return 1;
    }
    
    if (signal(SIGTERM, signal_handler) == SIG_ERR) {
        perror("Failed to set SIGTERM handler");
        return 1;
    }
    
    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        perror("Failed to set SIGUSR1 handler");
        return 1;
    }
    
    // Print process ID so we can send signals
    printf("Process ID: %d\n", getpid());
    printf("Try:\n");
    printf("  - Press Ctrl+C\n");
    printf("  - Run 'kill %d' in another terminal\n", getpid());
    printf("  - Run 'kill -USR1 %d' in another terminal\n", getpid());
    
    // Main loop
    while (keep_running) {
        printf("Running...\n");
        sleep(2);
    }
    
    printf("Program exited normally\n");
    return 0;
}
```

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

Example using sigaction() with siginfo:

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t sigusr1_count = 0;

// Extended signal handler with siginfo
void handler(int signo, siginfo_t *info, void *context) {
    // Safe to use write() in signal handlers
    const char *msg;
    
    switch (signo) {
        case SIGUSR1:
            sigusr1_count++;
            msg = "Received SIGUSR1\n";
            write(STDOUT_FILENO, msg, strlen(msg));
            
            // Get info about signal sender
            if (info != NULL) {
                char buffer[100];
                int len = snprintf(buffer, sizeof(buffer), 
                                 "Signal sent by process %d\n", info->si_pid);
                write(STDOUT_FILENO, buffer, len);
            }
            break;
            
        case SIGINT:
            msg = "\nReceived SIGINT. Exiting.\n";
            write(STDOUT_FILENO, msg, strlen(msg));
            exit(0);
            break;
    }
}

int main() {
    struct sigaction sa;
    
    // Initialize sigaction struct
    memset(&sa, 0, sizeof(sa));
    
    // We want to use the extended siginfo handler
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    
    // Block all other signals during handler
    sigfillset(&sa.sa_mask);
    
    // Register handlers
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Failed to set SIGUSR1 handler");
        return 1;
    }
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Failed to set SIGINT handler");
        return 1;
    }
    
    printf("Process ID: %d\n", getpid());
    printf("Send SIGUSR1 with: kill -USR1 %d\n", getpid());
    printf("Press Ctrl+C to exit\n");
    
    // Main loop
    while (1) {
        printf("SIGUSR1 count: %d\n", sigusr1_count);
        sleep(2);
    }
    
    return 0;
}
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

Example of sending SIGUSR1 to a process:

```c
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <PID>\n", argv[0]);
        return 1;
    }
    
    pid_t pid = atoi(argv[1]);
    
    // Send SIGUSR1 to the specified process
    if (kill(pid, SIGUSR1) == -1) {
        perror("Failed to send signal");
        return 1;
    }
    
    printf("Signal SIGUSR1 sent to process %d\n", pid);
    return 0;
}
```

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

#### Example: Communication Between Processes Using Signals

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void parent_handler(int signo) {
    if (signo == SIGUSR1)
        printf("Parent: Received SIGUSR1 from child\n");
}

void child_handler(int signo) {
    if (signo == SIGUSR2)
        printf("Child: Received SIGUSR2 from parent\n");
}

int main() {
    pid_t pid;
    
    // Set up signal handlers before fork
    signal(SIGUSR1, parent_handler);
    signal(SIGUSR2, child_handler);
    
    printf("Starting parent process (PID: %d)\n", getpid());
    
    // Create child process
    pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    
    if (pid == 0) {
        // Child process
        printf("Child process started (PID: %d)\n", getpid());
        
        // Send signal to parent
        printf("Child: Sending SIGUSR1 to parent\n");
        kill(getppid(), SIGUSR1);
        
        // Wait for signal from parent
        printf("Child: Waiting for signal...\n");
        sleep(2);
        
        printf("Child: Exiting\n");
        exit(0);
    } else {
        // Parent process
        printf("Parent: Child created with PID %d\n", pid);
        
        // Wait a moment for child to get ready
        sleep(1);
        
        // Send signal to child
        printf("Parent: Sending SIGUSR2 to child\n");
        kill(pid, SIGUSR2);
        
        // Wait for child to exit
        printf("Parent: Waiting for child to exit...\n");
        wait(NULL);
        
        printf("Parent: Child has exited. Parent exiting too.\n");
    }
    
    return 0;
}
```

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

#### Example: Protecting Critical Sections

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signo) {
    printf("\nSignal %d received\n", signo);
}

void critical_section(void) {
    sigset_t block_set, old_set;
    
    // Create set with important signals to block
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);
    sigaddset(&block_set, SIGTERM);
    
    printf("Entering critical section\n");
    
    // Block signals and save old mask
    sigprocmask(SIG_BLOCK, &block_set, &old_set);
    
    printf("Signals blocked. Try Ctrl+C now...\n");
    
    // Simulate important work
    for (int i = 5; i > 0; i--) {
        printf("%d seconds remaining in critical section\n", i);
        sleep(1);
    }
    
    // Check for pending signals
    sigset_t pending;
    sigpending(&pending);
    
    if (sigismember(&pending, SIGINT))
        printf("SIGINT is pending and will be delivered when unblocked\n");
        
    if (sigismember(&pending, SIGTERM))
        printf("SIGTERM is pending and will be delivered when unblocked\n");
    
    printf("Leaving critical section, unblocking signals\n");
    
    // Restore original signal mask
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    
    printf("Signals unblocked\n");
}

int main() {
    // Set up signal handlers
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    printf("Process ID: %d\n", getpid());
    
    // Run critical section
    critical_section();
    
    printf("Main program continues. Press Ctrl+C to exit.\n");
    
    while(1) {
        sleep(1);
    }
    
    return 0;
}
```