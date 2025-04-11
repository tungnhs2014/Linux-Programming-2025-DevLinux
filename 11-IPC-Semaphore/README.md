# IPC - Semaphores

## 11.1. Introduction

### 11.1.1. What is a Semaphore?

A Semaphore is an Inter-Process Communication (IPC) mechanism primarily used for **synchronization** between processes or threads. Its main purpose is to control access to shared resources, preventing race conditions and ensuring orderly execution. Think of it as a counter managed by the kernel that processes can atomically increment or decrement to signal availability or usage of a resource.

Key uses include:

*   **Mutual Exclusion (Mutex):** Preventing multiple processes/threads from accessing a critical section (like a shared memory region) simultaneously. A semaphore used this way is often called a **binary semaphore** (value 0 or 1).
*   **Resource Counting:** Limiting the number of processes/threads that can concurrently access a finite pool of resources (e.g., buffer slots, database connections). A semaphore used this way is called a **counting semaphore**.
*   **Signaling:** Allowing one process/thread to signal an event occurrence to another process/thread that might be waiting for it.

### 11.1.2. Semaphore Types

There are two main types (APIs) for semaphores on *nix systems:

1.  **System V Semaphores:** An older, more complex API, part of the System V IPC suite. Operates on *sets* of semaphores identified by a kernel-persistent ID.
2.  **POSIX Semaphores:** A newer, simpler, and generally preferred API. Comes in two flavors:
    *   **Named:** Identified by a name (like a file path, e.g., `/mysemaphore`). Accessible by unrelated processes.
    *   **Unnamed (Memory-Based):** Not identified by a name. Resides in memory shared between collaborating threads or processes (e.g., global variable for threads, shared memory segment for processes).

---

## 11.2. Core Semaphore Concepts and Operations

### 11.2.1. How Semaphores Work

At its core, a semaphore is typically an integer value maintained by the kernel. Its value is constrained to be non-negative (>= 0). Processes interact with the semaphore using atomic operations provided by the system APIs.

<p align="center">
  <img src="https://github.com/user-attachments/assets/16fda96a-d6b2-4e6c-a7ab-55fda8717ee6" alt="Semaphore Wait and Post Logic" width="600"/>
  <br/>
  <em>Figure 11.1: Conceptual flow of Semaphore Wait (P) and Post (V) operations.</em>
</p>

The two fundamental operations (often referred to by Dijkstra's original terms P and V) are:

*   **Wait (P operation, Decrement, `sem_wait` in POSIX):** Attempts to decrease the semaphore value by 1.
    *   If the current value is greater than 0, the decrement happens immediately, and the process continues.
    *   If the current value is 0, the process **blocks** (sleeps) until another process performs a Post operation. Once woken up, it attempts the decrement again.
*   **Post (V operation, Increment, `sem_post` in POSIX):** Increases the semaphore value by 1.
    *   If one or more processes are blocked waiting on this semaphore, one of them is woken up by the kernel to attempt the Wait operation again. The specific process woken depends on the kernel's scheduling policy (often FIFO, but not guaranteed).
    *   If no processes are waiting, the value is simply incremented.

### 11.2.2. The Wait (Decrement) Operation (POSIX Perspective)

The primary function for the Wait operation in POSIX semaphores is `sem_wait()`.

```c
#include <semaphore.h>

int sem_wait(sem_t *sem);
// Returns 0 on success, or -1 on error (sets errno).
```

*   It decrements the semaphore pointed to by `sem`.
*   If the semaphore's value is currently greater than zero, it returns immediately.
*   If the semaphore's value is zero, the call blocks until another process/thread calls `sem_post()` on the same semaphore, making the value positive again.
*   The decrement and the check-and-block are performed atomically.

**Non-blocking and Timed Variants:**

*   `sem_trywait()`: A non-blocking version. If decrementing would block (value is 0), it returns immediately with an error (`errno` set to `EAGAIN`) instead of blocking.
    ```c
    #include <semaphore.h>

    int sem_trywait(sem_t *sem);
    // Returns 0 on success, -1 on error.
    ```
*   `sem_timedwait()`: Blocks like `sem_wait()`, but only for a specified maximum time (`abs_timeout`). If the timeout expires before the semaphore can be decremented, it returns an error (`errno` set to `ETIMEDOUT`).
    ```c
    #define _POSIX_C_SOURCE 200809L // Or appropriate feature test macro
    #include <semaphore.h>
    #include <time.h>

    int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
    // Returns 0 on success, -1 on error.
    ```
    Note: `abs_timeout` specifies an absolute point in time (e.g., using `CLOCK_REALTIME`), not a relative duration.

### 11.2.3. The Post (Increment) Operation (POSIX Perspective)

The `sem_post()` function performs the Post operation.

```c
#include <semaphore.h>

int sem_post(sem_t *sem);
// Returns 0 on success, or -1 on error.
```

*   It increments the value of the semaphore pointed to by `sem`.
*   If the semaphore's value was zero before the call, and there are processes/threads blocked in `sem_wait()` on this semaphore, one of them will be unblocked.
*   The operation is atomic.

### 11.2.4. Getting the Semaphore Value (POSIX Perspective)

The `sem_getvalue()` function retrieves the current value of a semaphore.

```c
#include <semaphore.h>

int sem_getvalue(sem_t *sem, int *sval);
// Returns 0 on success, or -1 on error.
```

*   The current value of the semaphore `sem` is placed in the integer pointed to by `sval`.
*   **Caution:** The retrieved value might be outdated immediately after the call returns due to actions by other processes/threads. It's mainly useful for debugging or specific scenarios, not usually for synchronization logic itself.

---

## 11.3. System V Semaphores

System V semaphores are more complex than POSIX ones. They operate on **sets** of semaphores, identified by a system-wide integer ID (`semid`). Even if you only need one semaphore, you work with a set containing one member.

### 11.3.1. Overview

*   **Semaphore Sets:** A single `semid` refers to an array (set) of one or more semaphores. Each semaphore within the set is accessed by its index (starting from 0).
*   **Kernel Persistence:** Semaphore sets persist in the kernel even after the creating process exits, until explicitly removed using `semctl` with `IPC_RMID`.
*   **Identifiers:** Similar to System V Shared Memory, a `key_t` (often generated using `ftok()`) is used with `semget()` to obtain the `semid`.

### 11.3.2. Getting a Semaphore Set ID (`semget`)

`semget()` creates a new semaphore set or gets the ID of an existing one.

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semget(key_t key, int nsems, int semflg);
// Returns semid on success, -1 on error.
```

*   `key`: The `key_t` (e.g., from `ftok()`, or `IPC_PRIVATE`).
*   `nsems`: The number of semaphores in the set. **Required** when creating (`IPC_CREAT`). If getting an existing set ID, `nsems` can be 0 (or should match the existing set size, behavior varies).
*   `semflg`: Flags and permissions (bitwise OR):
    *   `IPC_CREAT`: Create the set if it doesn't exist.
    *   `IPC_EXCL`: Used with `IPC_CREAT`, fails if the set already exists.
    *   Permissions (e.g., `0666`): Control read/alter access to the semaphore set.

### 11.3.3. Semaphore Control Operations (`semctl`)

`semctl()` performs various control operations on the semaphore set or individual semaphores within it.

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// Definition might require _GNU_SOURCE or similar on some systems
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

int semctl(int semid, int semnum, int cmd, ... /* union semun arg */);
// Return value depends on cmd; typically 0 or a non-negative value on success, -1 on error.
```

*   `semid`: The semaphore set identifier from `semget()`.
*   `semnum`: The index of the semaphore within the set (0 to `nsems`-1) to operate on. Ignored by some commands that apply to the whole set.
*   `cmd`: The command to perform. Common commands:
    *   `SETVAL`: Sets the value of the single semaphore `semnum` to `arg.val`. **Crucial for initialization.**
    *   `GETVAL`: Returns the value of the single semaphore `semnum`.
    *   `SETALL`: Sets the values of all semaphores in the set from the array `arg.array`. Requires alter permission.
    *   `GETALL`: Gets the values of all semaphores into the array `arg.array`. Requires read permission.
    *   `IPC_STAT`: Gets the `semid_ds` structure (metadata) for the set into `arg.buf`. Requires read permission.
    *   `IPC_SET`: Sets permission-related fields of the `semid_ds` structure from `arg.buf`. Requires owner privileges.
    *   `IPC_RMID`: Marks the semaphore set for removal. The set is destroyed once the last process using it terminates or detaches. Requires owner privileges. This is the standard way to clean up System V semaphores.
*   `arg` (The fourth argument of type `union semun`): Its value/type depends on the `cmd`. **Note:** You often need to declare `union semun` yourself as it's not always defined in headers.

### 11.3.4. Semaphore Operations (`semop`)

`semop()` is the workhorse function for performing atomic Wait (P) and Post (V) operations on **one or more** semaphores within a set simultaneously.

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

struct sembuf {
    unsigned short sem_num;  /* Semaphore index in set (0, 1, ...) */
    short          sem_op;   /* Operation value */
    short          sem_flg;  /* Operation flags (e.g., IPC_NOWAIT, SEM_UNDO) */
};

int semop(int semid, struct sembuf *sops, size_t nsops);
// Returns 0 on success, -1 on error.
```

*   `semid`: The semaphore set identifier.
*   `sops`: A pointer to an array of `sembuf` structures. Each structure specifies one operation on one semaphore in the set.
*   `nsops`: The number of `sembuf` structures in the `sops` array.

**The `sembuf` structure:**

*   `sem_num`: Index of the semaphore within the set to operate on.
*   `sem_op`: The operation value:
    *   **Positive Value (`> 0`):** Increment (Post/V). Adds `sem_op` to the semaphore's value. Always proceeds immediately (never blocks). Useful for releasing multiple resources at once.
    *   **Negative Value (`< 0`):** Decrement (Wait/P). Attempts to subtract `abs(sem_op)` from the semaphore's value.
        *   If `semval >= abs(sem_op)`, the subtraction occurs, and the operation completes.
        *   If `semval < abs(sem_op)`, the operation blocks (unless `IPC_NOWAIT` is set) until the value increases sufficiently.
    *   **Zero Value (`== 0`):** Wait-for-zero. Blocks (unless `IPC_NOWAIT` is set) until the semaphore's value becomes exactly 0. Then proceeds without changing the value. Useful for waiting until a resource pool is completely free.
*   `sem_flg`: Flags:
    *   `IPC_NOWAIT`: If the operation would block, `semop` returns immediately with `errno` set to `EAGAIN`.
    *   `SEM_UNDO`: **Very Important!** If set, the kernel automatically reverses the effect of `sem_op` on this semaphore if the process terminates unexpectedly (e.g., crashes) before explicitly undoing the operation. This helps prevent resources from being permanently locked if a process holding a semaphore dies. Essential for robust mutex implementation.

**Atomicity:** All operations defined in the `sops` array (up to `nsops`) are performed **atomically** as a single unit. Either all succeed, or if any single operation would block (and `IPC_NOWAIT` is not set), the process blocks *before any* of the operations are applied. If any operation fails (e.g., permission error), none are applied.

### 11.3.5. System V Semaphore Example (Mutual Exclusion for Shared Counter)

This example uses a System V semaphore set (with one semaphore) as a mutex to protect access to a counter stored in System V shared memory.

**common_sysv.h:**

```c
#ifndef COMMON_SYSV_H
#define COMMON_SYSV_H

#include <sys/types.h>

#define SHM_KEY_PATH "/tmp/sysv_shm_key"
#define SEM_KEY_PATH "/tmp/sysv_sem_key"
#define PROJECT_ID 'X'
#define SHM_SIZE 1024

// Key for shared memory and semaphore set
key_t shm_key;
key_t sem_key;

// IDs
int shmid;
int semid;

// Pointer to shared memory
int *shared_counter;

// semop structures
struct sembuf wait_op = {0, -1, SEM_UNDO}; // Wait (P) on semaphore 0
struct sembuf post_op = {0, 1, SEM_UNDO};  // Post (V) on semaphore 0

// Required for semctl
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#endif // COMMON_SYSV_H
```

**shm_sem_writer_sysv.c (Increments counter):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include "common_sysv.h"

int main(int argc, char *argv[]) {
    int increments = (argc > 1) ? atoi(argv[1]) : 5; // Number of increments
    int i;

    // --- Get Keys ---
    shm_key = ftok(SHM_KEY_PATH, PROJECT_ID);
    sem_key = ftok(SEM_KEY_PATH, PROJECT_ID);
    if (shm_key == -1 || sem_key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // --- Get/Create Shared Memory ---
    shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // --- Attach Shared Memory ---
    shared_counter = (int *)shmat(shmid, NULL, 0);
    if (shared_counter == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE); // Cleanup might be needed here
    }

    // --- Get/Create Semaphore Set (1 semaphore) ---
    semid = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);
    int is_initializer = 0;
    if (semid != -1) {
        // We created it, so initialize
        is_initializer = 1;
        union semun init_arg;
        init_arg.val = 1; // Initialize mutex to 1 (available)
        if (semctl(semid, 0, SETVAL, init_arg) == -1) {
            perror("semctl SETVAL");
            shmdt(shared_counter);
            shmctl(shmid, IPC_RMID, NULL); // Clean up shm
            semctl(semid, 0, IPC_RMID);    // Clean up sem
            exit(EXIT_FAILURE);
        }
         *shared_counter = 0; // Initialize counter if we created semaphore
        printf("Writer (PID %d): Initialized semaphore %d and counter.\n", getpid(), semid);
    } else if (errno == EEXIST) {
        // Semaphore already exists, just get the ID
        semid = semget(sem_key, 1, 0666);
        if (semid == -1) {
            perror("semget (get existing)");
            shmdt(shared_counter);
            exit(EXIT_FAILURE);
        }
        printf("Writer (PID %d): Attached to existing semaphore %d.\n", getpid(), semid);
    } else {
        perror("semget (create/get)");
        shmdt(shared_counter);
        exit(EXIT_FAILURE);
    }


    // --- Critical Section Loop ---
    printf("Writer (PID %d): Incrementing counter %d times...\n", getpid(), increments);
    for (i = 0; i < increments; ++i) {
        // 1. Wait (P operation) - Acquire mutex
        if (semop(semid, &wait_op, 1) == -1) {
            perror("semop wait");
            break; // Exit loop on error
        }

        // --- Critical Section Start ---
        int current_val = *shared_counter;
        printf("Writer (PID %d): Read %d, Incrementing...\n", getpid(), current_val);
        usleep(10000); // Simulate some work
        *shared_counter = current_val + 1;
        printf("Writer (PID %d): Wrote %d\n", getpid(), *shared_counter);
        // --- Critical Section End ---

        // 2. Post (V operation) - Release mutex
        if (semop(semid, &post_op, 1) == -1) {
            perror("semop post");
            // Attempt to release mutex failed, state is uncertain
            break;
        }
        usleep(5000); // Small delay between increments
    }

    // --- Detach Shared Memory ---
    if (shmdt(shared_counter) == -1) {
        perror("shmdt");
    }

    // --- Optional: Remove Semaphore and Shared Memory ---
    // Typically, only one designated process (e.g., the initializer or a manager)
    // should remove the IPC objects when they are no longer needed by anyone.
    // if (is_initializer) { // Or based on some other logic
    //     printf("Writer (PID %d): Cleaning up IPC resources...\n", getpid());
    //     sleep(2); // Give others a chance to finish
    //     if (semctl(semid, 0, IPC_RMID) == -1) {
    //         perror("semctl IPC_RMID");
    //     }
    //     if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    //         perror("shmctl IPC_RMID");
    //     }
    // }

    printf("Writer (PID %d): Finished.\n", getpid());
    exit(EXIT_SUCCESS);
}

```

**shm_sem_reader_sysv.c (Decrements counter - similar structure):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include "common_sysv.h"

int main(int argc, char *argv[]) {
    int decrements = (argc > 1) ? atoi(argv[1]) : 3; // Number of decrements
    int i;

    // --- Get Keys ---
    shm_key = ftok(SHM_KEY_PATH, PROJECT_ID);
    sem_key = ftok(SEM_KEY_PATH, PROJECT_ID);
    if (shm_key == -1 || sem_key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // --- Get Shared Memory ---
    shmid = shmget(shm_key, SHM_SIZE, 0666); // Just get, don't create
    if (shmid == -1) {
        perror("shmget (Reader - check if writer ran first)");
        exit(EXIT_FAILURE);
    }

    // --- Attach Shared Memory ---
    shared_counter = (int *)shmat(shmid, NULL, 0);
    if (shared_counter == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // --- Get Semaphore Set ---
    semid = semget(sem_key, 1, 0666); // Just get, don't create
    if (semid == -1) {
        perror("semget (Reader - check if writer ran first and created sem)");
        shmdt(shared_counter);
        exit(EXIT_FAILURE);
    }
     printf("Reader (PID %d): Attached to existing semaphore %d.\n", getpid(), semid);

    // --- Critical Section Loop ---
     printf("Reader (PID %d): Decrementing counter %d times...\n", getpid(), decrements);
    for (i = 0; i < decrements; ++i) {
        // 1. Wait (P operation) - Acquire mutex
        printf("Reader (PID %d): Attempting to acquire lock...\n", getpid());
        if (semop(semid, &wait_op, 1) == -1) {
            perror("semop wait");
            break;
        }
         printf("Reader (PID %d): Lock acquired.\n", getpid());

        // --- Critical Section Start ---
        int current_val = *shared_counter;
        if (current_val > 0) {
            printf("Reader (PID %d): Read %d, Decrementing...\n", getpid(), current_val);
            usleep(15000); // Simulate some work
            *shared_counter = current_val - 1;
            printf("Reader (PID %d): Wrote %d\n", getpid(), *shared_counter);
        } else {
             printf("Reader (PID %d): Read %d, Counter is zero, not decrementing.\n", getpid(), current_val);
        }
        // --- Critical Section End ---

        // 2. Post (V operation) - Release mutex
        if (semop(semid, &post_op, 1) == -1) {
            perror("semop post");
            break;
        }
        printf("Reader (PID %d): Lock released.\n", getpid());
        usleep(8000); // Small delay
    }

    // --- Detach Shared Memory ---
    if (shmdt(shared_counter) == -1) {
        perror("shmdt");
    }

    printf("Reader (PID %d): Finished.\n", getpid());
    exit(EXIT_SUCCESS);
}
```

**How to Compile and Run System V Example:**

1.  Save the code: `common_sysv.h`, `shm_sem_writer_sysv.c`, `shm_sem_reader_sysv.c`.
2.  Create dummy files for `ftok`:
    ```bash
    touch /tmp/sysv_shm_key
    touch /tmp/sysv_sem_key
    ```
3.  Compile:
    ```bash
    gcc shm_sem_writer_sysv.c -o writer_sysv
    gcc shm_sem_reader_sysv.c -o reader_sysv
    ```
4.  Run (in separate terminals, potentially multiple instances):
    ```bash
    ./writer_sysv 10 # Run writer to increment 10 times (will initialize first time)
    ./reader_sysv 5  # Run reader to decrement 5 times
    ./writer_sysv 3  # Run another writer
    # ... etc.
    ```
5.  **Cleanup:** After finishing, manually remove the IPC objects (since the example code doesn't reliably do it):
    ```bash
    ipcrm -S $(ftok /tmp/sysv_sem_key X) # Remove semaphore set
    ipcrm -M $(ftok /tmp/sysv_shm_key X) # Remove shared memory segment
    # Or use ipcs to find IDs and ipcrm -s <semid> / ipcrm -m <shmid>
    ```

---

## 11.4. POSIX Semaphores

POSIX semaphores offer a simpler API compared to System V.

### 11.4.1. Overview

As mentioned, POSIX provides two types:

*   **Named Semaphores:** Identified by a string name (e.g., `/mysem`). Created using `sem_open()`. Accessible by any process that knows the name and has permissions. Persist in the system until explicitly unlinked (`sem_unlink()`).
*   **Unnamed Semaphores:** Reside in process memory. Initialized using `sem_init()`. For use between threads of the *same* process, they can be in global or heap memory. For use between *different* processes, the `sem_t` variable itself **must** be placed in a shared memory region (created via System V or POSIX shared memory APIs). They cease to exist when the memory they occupy is released/unmapped, or they are explicitly destroyed (`sem_destroy()`).

### 11.4.2. Named Semaphores

Ideal for synchronization between unrelated processes.

#### 11.4.2.1. Creating/Opening (`sem_open`)

`sem_open()` creates a new named semaphore or opens an existing one.

```c
#include <fcntl.h>      /* Defines O_* constants */
#include <sys/stat.h>   /* Defines mode constants */
#include <semaphore.h>

sem_t *sem_open(const char *name, int oflag, /* mode_t mode, unsigned int value */);
// Returns pointer to semaphore on success, or SEM_FAILED on error.
```

*   `name`: Identifier string, typically starting with `/` (e.g., `/my_app_semaphore`). Kernel usually maps this to a file under `/dev/shm/` or similar.
*   `oflag`: Specifies the operation mode:
    *   `0`: Open an existing semaphore. Fails if it doesn't exist.
    *   `O_CREAT`: Create a new semaphore if it doesn't exist. If it exists, open it.
    *   `O_CREAT | O_EXCL`: Create a new semaphore. Fails if a semaphore with `name` already exists. Useful for ensuring only one process initializes it.
*   **Arguments required only if `O_CREAT` is specified:**
    *   `mode`: Permissions for the new semaphore (e.g., `0660` for read/write by user and group), combined with the process's umask. See permission bits table below.
    *   `value`: The initial integer value (>= 0) for the new semaphore. Often 1 for a mutex.

**Permission Mode Constants (`mode_t`):**

| Constant | Octal value | Permission bit     |
|----------|-------------|---------------------|
| S_IRUSR  | 0400        | User-read           |
| S_IWUSR  | 0200        | User-write          |
| S_IXUSR  | 0100        | User-execute        |
| S_IRGRP  | 040         | Group-read          |
| S_IWGRP  | 020         | Group-write         |
| S_IXGRP  | 010         | Group-execute       |
| S_IROTH  | 04          | Other-read          |
| S_IWOTH  | 02          | Other-write         |
| S_IXOTH  | 01          | Other-execute       |
| *(Other constants like S_ISUID, S_ISGID, S_ISVTX are usually not relevant for semaphores)* | | |

#### 11.4.2.2. Closing (`sem_close`)

`sem_close()` informs the system that the calling process is finished using the semaphore associated with the pointer `sem`.

```c
#include <semaphore.h>

int sem_close(sem_t *sem);
// Returns 0 on success, or -1 on error.
```

*   It decreases the system's reference count for this named semaphore associated with the process.
*   It does **not** remove the semaphore from the system. The name still exists.
*   Frees resources allocated by `sem_open()` within the calling process.

#### 11.4.2.3. Removing (`sem_unlink`)

`sem_unlink()` removes the name of the semaphore from the system.

```c
#include <semaphore.h>

int sem_unlink(const char *name);
// Returns 0 on success, or -1 on error.
```

*   It marks the semaphore for destruction.
*   The actual destruction happens only after all processes that have the semaphore open call `sem_close()`.
*   Crucial for cleanup to prevent orphaned semaphores. Should typically be called by one designated process (e.g., the initializer or a cleanup script) when the semaphore is no longer needed system-wide.

#### 11.4.2.4. Named Semaphore Example (Simple Ping-Pong)

Process A waits, Process B posts, then B waits and A posts.

**common_posix_named.h:**

```c
#ifndef COMMON_POSIX_NAMED_H
#define COMMON_POSIX_NAMED_H

#define SEM_A_NAME "/sem_a_pingpong"
#define SEM_B_NAME "/sem_b_pingpong"

#endif // COMMON_POSIX_NAMED_H
```

**pingpong_a_named.c:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h> // For sleep
#include "common_posix_named.h"

int main() {
    sem_t *sem_a, *sem_b;

    // Create/Open semaphores. Initialize to 0 (locked). Process A starts blocked.
    // Permissions 0660: User and Group read/write
    sem_a = sem_open(SEM_A_NAME, O_CREAT, 0660, 0);
    sem_b = sem_open(SEM_B_NAME, O_CREAT, 0660, 0);

    if (sem_a == SEM_FAILED || sem_b == SEM_FAILED) {
        perror("sem_open");
        // Attempt cleanup if possible
        if (sem_a != SEM_FAILED) sem_close(sem_a);
        if (sem_b != SEM_FAILED) sem_close(sem_b);
        // Unlink might be needed if created partially
        sem_unlink(SEM_A_NAME);
        sem_unlink(SEM_B_NAME);
        exit(EXIT_FAILURE);
    }

    printf("Process A (PID %d): Ready.\n", getpid());

    // --- Loop ---
    for (int i = 0; i < 5; ++i) {
        printf("Process A: Waiting for Process B (sem_wait on %s)...\n", SEM_A_NAME);
        if (sem_wait(sem_a) == -1) { // Wait for B's signal
            perror("sem_wait sem_a");
            break;
        }

        printf("Process A: Got signal from B! Doing work...\n");
        sleep(1); // Simulate work

        printf("Process A: Signaling Process B (sem_post on %s)...\n", SEM_B_NAME);
        if (sem_post(sem_b) == -1) { // Signal B
            perror("sem_post sem_b");
            break;
        }
    }

    // --- Cleanup ---
    printf("Process A: Closing semaphores.\n");
    sem_close(sem_a);
    sem_close(sem_b);

    // One process should ideally unlink. Let's assume A does.
    printf("Process A: Unlinking semaphores.\n");
    sem_unlink(SEM_A_NAME);
    sem_unlink(SEM_B_NAME);

    printf("Process A: Finished.\n");
    exit(EXIT_SUCCESS);
}
```

**pingpong_b_named.c:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h> // For sleep
#include "common_posix_named.h"

int main() {
    sem_t *sem_a, *sem_b;

    // Open existing semaphores (don't use O_CREAT here ideally)
    // Note: If B starts before A, this might fail. Robust apps need better init.
    sem_a = sem_open(SEM_A_NAME, 0); // Just open
    sem_b = sem_open(SEM_B_NAME, 0); // Just open

    if (sem_a == SEM_FAILED || sem_b == SEM_FAILED) {
        perror("sem_open (Process B - Did Process A run first?)");
        if (sem_a != SEM_FAILED) sem_close(sem_a);
        if (sem_b != SEM_FAILED) sem_close(sem_b);
        exit(EXIT_FAILURE);
    }

    printf("Process B (PID %d): Ready.\n", getpid());

    // --- Loop ---
    for (int i = 0; i < 5; ++i) {
        printf("Process B: Doing work before signaling A...\n");
        sleep(1); // Simulate work

        printf("Process B: Signaling Process A (sem_post on %s)...\n", SEM_A_NAME);
        if (sem_post(sem_a) == -1) { // Signal A
            perror("sem_post sem_a");
            break;
        }

        printf("Process B: Waiting for Process A (sem_wait on %s)...\n", SEM_B_NAME);
        if (sem_wait(sem_b) == -1) { // Wait for A's signal
            perror("sem_wait sem_b");
            break;
        }
        printf("Process B: Got signal from A!\n");
    }

    // --- Cleanup ---
    printf("Process B: Closing semaphores.\n");
    sem_close(sem_a);
    sem_close(sem_b);

    // B does not unlink, assumes A does.

    printf("Process B: Finished.\n");
    exit(EXIT_SUCCESS);
}
```

**How to Compile and Run Named POSIX Example:**

1.  Save code: `common_posix_named.h`, `pingpong_a_named.c`, `pingpong_b_named.c`.
2.  Compile (link with `-lpthread`, although not strictly necessary for semaphores alone on modern Linux, it's good practice; `-lrt` might be needed on older systems):
    ```bash
    gcc pingpong_a_named.c -o proc_a -lpthread # -lrt if needed
    gcc pingpong_b_named.c -o proc_b -lpthread # -lrt if needed
    ```
3.  Run in separate terminals:
    ```bash
    ./proc_a & # Run A in background (or separate terminal)
    ./proc_b   # Run B in foreground
    ```
    You should see them alternating signals. Process A will handle the `sem_unlink` at the end.

### 11.4.3. Unnamed Semaphores (Memory-Based)

Suitable for threads within the same process or for processes that already share a memory region.

#### 11.4.3.1. Initialization (`sem_init`)

`sem_init()` initializes an unnamed semaphore located at address `sem`.

```c
#include <semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value);
// Returns 0 on success, or -1 on error.
```

*   `sem`: Pointer to the `sem_t` object to initialize. This object **must** reside in memory accessible to all intended users.
*   `pshared`: Determines sharing behavior:
    *   `0`: The semaphore is shared between **threads** of the process that allocated the memory for `sem`. `sem` can point to global or heap memory.
    *   **Non-zero:** The semaphore is shared between **processes**. `sem` **must** point to a location within a shared memory region (e.g., created using `shmget`/`shmat` or `shm_open`/`mmap`).
*   `value`: The initial integer value (>= 0) for the semaphore.

#### 11.4.3.2. Destruction (`sem_destroy`)

`sem_destroy()` destroys an unnamed semaphore.

```c
#include <semaphore.h>

int sem_destroy(sem_t *sem);
// Returns 0 on success, or -1 on error.
```

*   Frees resources associated with the semaphore `sem`.
*   Only call this after ensuring no thread/process is currently using or blocked on the semaphore.
*   Behavior is undefined if `sem_destroy` is called on a semaphore that other threads/processes are using.
*   After destruction, the `sem_t` memory can be reused or freed. You can re-initialize it later using `sem_init()`.

#### 11.4.3.3. Unnamed Semaphore Example (Inter-Process using POSIX Shared Memory)

This example uses an unnamed semaphore placed in POSIX shared memory to synchronize access to a shared resource (simulated by printing).

**common_posix_unnamed.h:**

```c
#ifndef COMMON_POSIX_UNNAMED_H
#define COMMON_POSIX_UNNAMED_H

#include <semaphore.h>

#define SHM_NAME "/unnamed_sem_shm"
#define SHM_SIZE sizeof(sem_t) // Just enough space for the semaphore

#endif // COMMON_POSIX_UNNAMED_H
```

**unnamed_proc1.c (Initializes and Uses):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common_posix_unnamed.h"

int main() {
    int fd;
    sem_t *sem_ptr;

    // 1. Create/Open Shared Memory
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0660);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // 2. Set Shared Memory Size
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    // 3. Map Shared Memory
    sem_ptr = (sem_t *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sem_ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    // Can close fd after mapping
    close(fd);

    // 4. Initialize Unnamed Semaphore in Shared Memory
    // pshared != 0 because it's for inter-process sharing
    // Initialize value to 1 (mutex)
    if (sem_init(sem_ptr, 1, 1) == -1) {
         // Check if already initialized (by another process that crashed?)
         // This simple example assumes it's the first time or cleanup failed.
         // Robust apps need better init coordination or check errno.
        perror("sem_init");
        munmap(sem_ptr, SHM_SIZE);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    printf("Process 1 (PID %d): Initialized unnamed semaphore in shared memory.\n", getpid());


    // 5. Use the semaphore
    for (int i = 0; i < 3; ++i) {
        printf("Process 1 (PID %d): Attempting lock...\n", getpid());
        if (sem_wait(sem_ptr) == -1) {
            perror("sem_wait");
            break;
        }
        printf("Process 1 (PID %d): >>> Acquired Lock. Critical Section Start.\n", getpid());
        sleep(2); // Simulate work
        printf("Process 1 (PID %d): <<< Critical Section End. Releasing Lock.\n", getpid());
        if (sem_post(sem_ptr) == -1) {
            perror("sem_post");
            break; // Don't continue if post fails
        }
        usleep(50000); // Small delay
    }

    // 6. Destroy Semaphore (should ideally be done by the last process)
    printf("Process 1 (PID %d): Destroying semaphore...\n", getpid());
    if (sem_destroy(sem_ptr) == -1) {
         perror("sem_destroy");
         // May fail if other process is still using it
    }


    // 7. Unmap and Unlink Shared Memory
    munmap(sem_ptr, SHM_SIZE);
    shm_unlink(SHM_NAME); // Mark shared memory for removal

    printf("Process 1 (PID %d): Finished.\n", getpid());
    exit(EXIT_SUCCESS);
}
```

**unnamed_proc2.c (Uses Existing):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include "common_posix_unnamed.h"

int main() {
    int fd;
    sem_t *sem_ptr;

    // 1. Open Existing Shared Memory
    fd = shm_open(SHM_NAME, O_RDWR, 0); // Open R/W, don't create
    if (fd == -1) {
        perror("shm_open (Proc 2 - Did Proc 1 run first?)");
        exit(EXIT_FAILURE);
    }

    // 2. No ftruncate needed when opening existing

    // 3. Map Shared Memory
    sem_ptr = (sem_t *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sem_ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    // 4. DO NOT sem_init() here! Use the semaphore initialized by Proc 1.
    printf("Process 2 (PID %d): Mapped shared memory containing semaphore.\n", getpid());

    // 5. Use the semaphore
    for (int i = 0; i < 3; ++i) {
         printf("Process 2 (PID %d): Attempting lock...\n", getpid());
        if (sem_wait(sem_ptr) == -1) {
            perror("sem_wait");
            break;
        }
        printf("Process 2 (PID %d): >>> Acquired Lock. Critical Section Start.\n", getpid());
        sleep(1); // Simulate work
        printf("Process 2 (PID %d): <<< Critical Section End. Releasing Lock.\n", getpid());
        if (sem_post(sem_ptr) == -1) {
            perror("sem_post");
            break;
        }
         usleep(70000); // Small delay
    }

    // 6. DO NOT sem_destroy() here generally. Let the initializer/manager handle it.

    // 7. Unmap Shared Memory
    munmap(sem_ptr, SHM_SIZE);

    printf("Process 2 (PID %d): Finished.\n", getpid());
    exit(EXIT_SUCCESS);
}
```

**How to Compile and Run Unnamed POSIX Example:**

1.  Save code: `common_posix_unnamed.h`, `unnamed_proc1.c`, `unnamed_proc2.c`.
2.  Compile (link with `-lpthread`, and `-lrt` for `shm_open`/`shm_unlink`):
    ```bash
    gcc unnamed_proc1.c -o proc1_unnamed -lpthread -lrt
    gcc unnamed_proc2.c -o proc2_unnamed -lpthread -lrt
    ```
3.  Run in separate terminals (Proc 1 must run first to initialize):
    ```bash
    ./proc1_unnamed & # Run initializer in background
    ./proc2_unnamed   # Run second process
    # Run more instances of ./proc2_unnamed if desired
    ```
    Observe how they take turns accessing the critical section. Process 1 handles the cleanup (`sem_destroy`, `shm_unlink`).

---

## 11.5. System V vs. POSIX Semaphores

| Feature           | System V Semaphores                      | POSIX Semaphores                         |
|-------------------|------------------------------------------|------------------------------------------|
| **Unit**          | Sets of semaphores                       | Single semaphores                        |
| **Identification**| `key_t` -> `semid` (integer)             | Named: `/name` string; Unnamed: `sem_t*` |
| **API Complexity**| Higher (`semget`, `semctl`, `semop`)     | Lower (`sem_open`/`sem_init`, `sem_wait`, `sem_post`, `sem_close`/`sem_destroy`, `sem_unlink`) |
| **Operations**    | `semop` allows complex atomic ops on multiple semaphores in the set | Simple wait/post on single semaphore   |
| **Persistence**   | Kernel persistent until `semctl(IPC_RMID)` | Named: Kernel persistent until `sem_unlink()`; Unnamed: Lifetime tied to memory location |
| **Undo Feature**  | `SEM_UNDO` flag in `semop` (robust)    | No built-in equivalent                   |
| **Portability**   | Standard on most Unix-like systems       | Generally preferred for new development, wider standard compliance (POSIX) |
| **Limits**        | System-wide limits on sets & semaphores | Limits on named semaphores; unnamed limited by memory |

**Recommendation:** For new applications, **POSIX semaphores** (especially named ones for unrelated processes, or unnamed ones in shared memory for related processes) are generally simpler and preferred unless you specifically need the complex atomic operations or `SEM_UNDO` feature of System V's `semop`.

---

## 11.6. Important Considerations

*   **Atomicity:** Ensure you understand which operations are atomic. Semaphore operations themselves (`sem_wait`, `sem_post`, `semop`) are atomic, but the code *between* acquiring and releasing a semaphore is not inherently atomic with respect to other processes unless protected by the semaphore.
*   **Initialization:** Correct initialization is crucial. Ensure only one process initializes a semaphore (using `O_EXCL` flags or other coordination). Set the correct initial value (e.g., 1 for mutex, N for resource pool). Unnamed semaphores in shared memory must be initialized *after* the memory is mapped.
*   **Cleanup:** Failure to clean up persistent semaphores (System V sets via `semctl(IPC_RMID)`, POSIX named via `sem_unlink()`) leads to resource leaks in the kernel. Unnamed semaphores need `sem_destroy()`. `SEM_UNDO` is vital for System V robustness against crashes.
*   **Error Handling:** Always check the return values of semaphore functions and handle errors appropriately (e.g., check `errno`).
*   **Deadlock:** Be cautious when using multiple semaphores. Acquiring them in different orders in different processes can lead to deadlock (e.g., Process A waits on Sem1 then Sem2, Process B waits on Sem2 then Sem1). Always acquire multiple locks in a consistent order.
*   **Linking:** Remember to link with `-lpthread` when using POSIX semaphores, and potentially `-lrt` for named POSIX semaphores or POSIX shared memory functions (`shm_open`, `shm_unlink`). System V functions usually don't require special linking.

---
