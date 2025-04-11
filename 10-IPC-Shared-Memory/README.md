# IPC - Shared Memory

## 10.1. Introduction

### 10.1.1. What is IPC?

IPC (Inter-Process Communication) refers to the mechanisms and techniques that allow independent processes to communicate, exchange data, and synchronize their actions within an operating system.

The main aspects of IPC include:

*   **Data Transfer/Sharing:** Enables one process to send information to one or more other processes.
*   **Synchronization:** Ensures that processes access shared resources (like shared memory) in an orderly manner, preventing data conflicts (race conditions).

**Examples of common IPC mechanisms:** Pipes, FIFOs, Message Queues, Semaphores, Shared Memory, Sockets.

<p align="center">
  <img src="https://github.com/user-attachments/assets/3c5fb086-13c9-4faa-8a4c-cade552614a9" alt="IPC Mechanisms Diagram" width="500"/>
  <br/>
  <em>Figure 1: Model of Inter-Process Communication (IPC)</em>
</p>

### 10.1.2. What is Shared Memory?

Shared Memory is an efficient IPC mechanism that allows two or more processes to **directly access the same region of physical memory (RAM)**. The kernel allocates a memory segment and then maps this segment into the virtual address space of the processes requesting access.

<p align="center">
  <img src="https://github.com/user-attachments/assets/3125df9c-68b6-40ec-b170-1964834de116" alt="Process Virtual Address Space Diagram" width="500"/>
  <br/>
  <em>Figure 2: Illustration of a process's virtual address space. The Shared Memory/Mapped region is typically located in the higher part of the address space, separate from the Stack and Heap.</em>
</p>

When one process writes data to the shared memory region, other processes that have mapped this region can immediately see the changes **without involving system calls to copy data** into or out of the kernel space.

There are two main API standards for working with shared memory on *nix systems:

1.  **System V Shared Memory:** An older standard, part of the System V IPC suite (along with System V Semaphores and Message Queues). Uses functions like `shmget`, `shmat`, `shmdt`, `shmctl`.
2.  **POSIX Shared Memory:** A newer, more modern, and flexible standard. Often preferred. Uses functions like `shm_open`, `ftruncate`, `mmap`, `munmap`, `shm_unlink`.

---

## 10.2. Why Use Shared Memory?

Shared Memory is the IPC mechanism of choice when **data exchange speed** is the most critical factor.

*   **Superior Speed:** It is the **fastest** IPC method for transferring large amounts of data between processes. Since processes access the same physical memory directly, data doesn't need to be copied between user space and kernel space, or between processes as in other mechanisms (like pipes or message queues).
*   **Resource Efficiency:** Minimizes CPU and memory overhead associated with data copying.
*   **Flexibility:** Allows complex data structures to be built directly in the shared memory region for multiple processes to use concurrently.

**However, Shared Memory also has drawbacks:**

*   **Complex Synchronization:** Shared Memory **does not provide built-in synchronization mechanisms**. Processes must manage concurrent access themselves using other IPC tools like Semaphores (System V or POSIX) or Mutexes (often used with Pthreads and can be placed in shared memory) to avoid race conditions (where data might be overwritten or read inconsistently).
*   **Management:** Careful management (creation, destruction) of shared memory objects is required to prevent resource leaks.

---

## 10.3. System V Shared Memory

System V Shared Memory manages shared memory regions using system-wide unique integer identifiers.

**Key Implementation Steps:**

1.  **Create or Get Key:** Use `ftok()` to generate a unique `key_t` based on an existing file path and a project ID. This key is used by related processes to refer to the same shared memory segment.
2.  **Create or Get Segment ID:** Use `shmget()` with the `key_t` to request the kernel to create a new shared memory segment or get the ID (`shmid`) of an existing one. Size and access permissions are specified here.
3.  **Attach Segment:** Use `shmat()` with the `shmid` to map the shared memory segment into the calling process's virtual address space. This returns a `void*` pointer to the beginning of the shared memory region within the process's address space.
4.  **Use Segment:** Read/write data to the shared memory region via the pointer obtained from `shmat()`. **(External synchronization mechanism is required!)**
5.  **Detach Segment:** Use `shmdt()` with the pointer returned by `shmat()` to unmap the segment from the process's address space.
6.  **Control and Release Segment:** Use `shmctl()` with the `shmid` and commands like `IPC_STAT` (get info), `IPC_SET` (change permissions), or `IPC_RMID` (mark segment for removal). The segment is only actually removed from the system after the last process detaches from it *and* a process has called `shmctl` with `IPC_RMID`.

### 10.3.1. Create Key (`ftok`)

The `ftok()` function generates a `key_t` key based on the `pathname` of an existing file and a `proj_id` (a non-zero integer). Different processes can generate the same `key_t` if they use the same `pathname` and `proj_id`.

```c
#include <sys/ipc.h>
#include <sys/shm.h>

key_t ftok(const char *pathname, int proj_id);
// Returns key_t on success, (key_t)-1 on error.
```

### 10.3.2. Create/Get Shared Memory Segment (`shmget`)

The `shmget()` function is used to create a new segment or get the ID of an existing one.

```c
#include <sys/ipc.h>
#include <sys/shm.h>

int shmget(key_t key, size_t size, int shmflg);
// Returns shmid (segment identifier) on success, -1 on error.
```

*   `key`: The `key_t` from `ftok()` or `IPC_PRIVATE`.
*   `size`: The size (in bytes) of the segment to create. If getting an existing segment, `size` can be 0.
*   `shmflg`: Control flags and access permissions.
    *   `IPC_CREAT`: Create the segment if it doesn't exist.
    *   `IPC_EXCL`: Used with `IPC_CREAT`, causes an error if the segment already exists.
    *   Access Permissions (e.g., `0666` for read/write by user, group, others):
        | Bit  | Meaning          |
        |------|------------------|
        | 0400 | Read by user     |
        | 0200 | Write by user    |
        | 0040 | Read by group    |
        | 0020 | Write by group   |
        | 0004 | Read by others   |
        | 0002 | Write by others  |

### 10.3.3. Attach/Detach Shared Memory Segment (`shmat`, `shmdt`)

`shmat()` maps the segment into the address space. `shmdt()` unmaps it.

```c
#include <sys/types.h>
#include <sys/shm.h>

void *shmat(int shmid, const void *shmaddr, int shmflg);
// Returns pointer to memory region on success, (void *)-1 on error.

int shmdt(const void *shmaddr);
// Returns 0 on success, -1 on error.
```

*   `shmid`: Segment ID obtained from `shmget()`.
*   `shmaddr` (for `shmat`): Suggested address to map at. Usually `NULL` to let the system choose.
*   `shmflg` (for `shmat`): Flags, e.g., `SHM_RDONLY` for read-only access.
*   `shmaddr` (for `shmdt`): The pointer returned by `shmat()`.

<p align="center">
  <img src="https://github.com/user-attachments/assets/f587682f-4116-4056-a16b-c0e866d77a53" width="400"/>
  <br/>
  <em>Figure 3: Illustration of `shmat` mapping a segment into a process's address space.</em>
</p>

Calling `shmdt()` does not remove the segment from the system; it only detaches it from the current process.

### 10.3.4. Control and Release Shared Memory Segment (`shmctl`)

`shmctl()` performs control operations on the segment.

```c
#include <sys/ipc.h>
#include <sys/shm.h>

int shmctl(int shmid, int cmd, struct shmid_ds *buf);
// Returns 0 on success, -1 on error.
```

*   `shmid`: Segment ID.
*   `cmd`: Command to execute:
    *   `IPC_STAT`: Get segment status information, store in `buf` (type `struct shmid_ds`).
    *   `IPC_SET`: Set access permissions, user ID, group ID from `buf`.
    *   `IPC_RMID`: Mark the segment for removal. The kernel removes it when the number of attached processes becomes zero.
*   `buf`: Pointer to a `shmid_ds` structure to receive or provide information.

```c
// Structure containing segment information (used with IPC_STAT, IPC_SET)
struct shmid_ds {
    struct ipc_perm shm_perm;  /* Access permissions (owner, group, mode) */
    size_t          shm_segsz; /* Size of segment (bytes) */
    pid_t           shm_lpid;  /* PID of last process performing shmop() */
    pid_t           shm_cpid;  /* PID of creator */
    shmatt_t        shm_nattch;/* Number of current attaches */
    time_t          shm_atime; /* Last attach time */
    time_t          shm_dtime; /* Last detach time */
    time_t          shm_ctime; /* Last change time */
    // ...
};

// Structure containing permission info (within shmid_ds)
struct ipc_perm {
    uid_t    uid;    /* Owner's user ID */
    gid_t    gid;    /* Owner's group ID */
    uid_t    cuid;   /* Creator's user ID */
    gid_t    cgid;   /* Creator's group ID */
    mode_t   mode;   /* Access modes (bits like 0666) */
    // ...
};
```

<p align="center">
  <img src="https://github.com/user-attachments/assets/c34e8c29-047c-468b-9bdd-36a9807ce32e" width="400"/>
  <br/>
  <em>Figure 4: Illustration of the `shmctl` function.</em>
</p>

### 10.3.5. System V Shared Memory Example

**shm_writer_sysv.c (Writing Process):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h> // For sleep

#define SHM_SIZE 1024       // Shared memory size
#define SHM_KEY_PATH "/tmp" // File path for ftok
#define SHM_KEY_ID 'A'      // Project ID for ftok

int main() {
    key_t key;
    int shmid;
    char *shm_ptr;
    char *data_to_write = "Hello from System V Writer!";

    // 1. Create key
    key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    printf("Writer: Generated key: %d\n", key);

    // 2. Create segment (or get if exists)
    // Create new with 0666 permissions (read/write for everyone)
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    printf("Writer: Shared memory segment created/obtained. ID: %d\n", shmid);

    // 3. Attach segment to address space
    shm_ptr = (char *)shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat");
        // Consider removing segment if creator and attach failed
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    printf("Writer: Shared memory attached at address: %p\n", shm_ptr);

    // 4. Write data to shared memory
    printf("Writer: Writing data: \"%s\"\n", data_to_write);
    strncpy(shm_ptr, data_to_write, SHM_SIZE - 1);
    shm_ptr[SHM_SIZE - 1] = '\0'; // Ensure null-termination

    // Wait a bit for the reader (real applications need proper synchronization)
    printf("Writer: Data written. Waiting for reader...\n");
    sleep(10); // Wait 10 seconds

    // 5. Detach segment
    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        // Still try to remove the segment
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    printf("Writer: Shared memory detached.\n");

    // 6. Mark segment for removal (if you are the last one, it will be removed)
    // Typically, a "server" or manager process does this
    // after knowing no one else needs it. In this simple example, the writer removes it.
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl(IPC_RMID)");
        // exit(EXIT_FAILURE); // Not strictly necessary to exit on removal error
    }
    printf("Writer: Marked segment for removal.\n");

    exit(EXIT_SUCCESS);
}
```

**shm_reader_sysv.c (Reading Process):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h> // For sleep

#define SHM_SIZE 1024
#define SHM_KEY_PATH "/tmp"
#define SHM_KEY_ID 'A'

int main() {
    key_t key;
    int shmid;
    char *shm_ptr;

    // 1. Generate the key (must be identical to writer)
    key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    printf("Reader: Generated key: %d\n", key);

    // 2. Get the ID of the segment created by the writer
    // Permissions are not strictly needed when just getting the ID, but size must match
    // or be 0. The 0666 flags here just ensure access if the writer isn't finished.
    shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget (Reader might start before writer? Check key/perms)");
        exit(EXIT_FAILURE);
    }
    printf("Reader: Shared memory segment ID: %d\n", shmid);

    // 3. Attach the segment
    shm_ptr = (char *)shmat(shmid, NULL, 0); // Attach for read/write (or SHM_RDONLY)
    if (shm_ptr == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    printf("Reader: Shared memory attached at address: %p\n", shm_ptr);

    // 4. Read data from shared memory
    printf("Reader: Waiting a bit before reading...\n");
    sleep(1); // Wait for writer (needs better synchronization)
    printf("Reader: Data read from shared memory: \"%s\"\n", shm_ptr);

    // 5. Detach the segment
    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    printf("Reader: Shared memory detached.\n");

    // The reader usually doesn't call IPC_RMID. Removal is handled
    // by the writer or a dedicated manager process.

    exit(EXIT_SUCCESS);
}
```

**How to Compile and Run:**

1.  Save the code into two files: `shm_writer_sysv.c` and `shm_reader_sysv.c`.
2.  Create the dummy file for `ftok`: `touch /tmp` (if `/tmp` doesn't exist or isn't a regular file/directory).
3.  Compile:
    ```bash
    gcc shm_writer_sysv.c -o writer_sysv
    gcc shm_reader_sysv.c -o reader_sysv
    ```
4.  Run the writer in one terminal: `./writer_sysv`
5.  Run the reader in another terminal (within 10 seconds after the writer writes): `./reader_sysv`

---

## 10.4. POSIX Shared Memory

POSIX Shared Memory uses a model based on file descriptors and memory mapping (`mmap`). Shared memory objects are identified by names (like filenames, typically starting with `/`).

**Key Implementation Steps:**

1.  **Create/Open Shared Memory Object:** Use `shm_open()` with a unique name (e.g., `/my_shared_mem`) to create a new or open an existing shared memory object. This returns a file descriptor (`fd`).
2.  **Set Size:** **(Only when creating new)** Use `ftruncate()` with the `fd` and the desired size to allocate space for the shared memory object. Newly created objects have a size of 0.
3.  **Map Object:** Use `mmap()` with the `fd` to map the shared memory object into the process's virtual address space. This returns a `void*` pointer to the memory region.
4.  **Use Memory Region:** Read/write data via the pointer from `mmap()`. **(External synchronization mechanism is required!)**
5.  **Unmap Object:** Use `munmap()` with the pointer and the mapped size to unmap the region.
6.  **Close File Descriptor:** Use `close()` to close the file descriptor returned by `shm_open()`.
7.  **Unlink Object:** Use `shm_unlink()` with the object's name to remove the name from the system. The shared memory object itself is destroyed only when all processes have `close()`d the file descriptor *and* `munmap()`ped it, *and* `shm_unlink()` has been called.

### 10.4.1. Create/Open Shared Memory Object (`shm_open`)

```c
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */

int shm_open(const char *name, int oflag, mode_t mode);
// Returns file descriptor on success, -1 on error.
```

*   `name`: Name of the object, must start with `/` and contain no other `/` (e.g., `/my_shm`).
*   `oflag`: File opening flags (similar to `open()`):
    *   `O_RDONLY`: Open for reading only.
    *   `O_RDWR`: Open for reading and writing.
    *   `O_CREAT`: Create the object if it doesn't exist.
    *   `O_EXCL`: Used with `O_CREAT`, causes an error if the object already exists.
    *   `O_TRUNC`: If the object exists, truncate its contents to zero length.
*   `mode`: Access permissions (e.g., `0660`) if creating a new object (combined with `umask`).

### 10.4.2. Set Size (`ftruncate`)

Necessary after creating a new object using `shm_open` with `O_CREAT`.

```c
#include <unistd.h>
#include <sys/types.h>

int ftruncate(int fd, off_t length);
// Returns 0 on success, -1 on error.
```

*   `fd`: File descriptor from `shm_open()`.
*   `length`: Desired size (in bytes).

### 10.4.3. Map/Unmap Shared Memory Object (`mmap`, `munmap`)

`mmap()` maps the object into memory, `munmap()` unmaps it.

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
// Returns pointer to mapped area on success, MAP_FAILED ((void *)-1) on error.

int munmap(void *addr, size_t length);
// Returns 0 on success, -1 on error.
```

*   `addr` (for `mmap`): Suggested starting address, usually `NULL`.
*   `length`: Size of the region to map (must be less than or equal to the size set by `ftruncate`).
*   `prot`: Memory protection flags: `PROT_READ`, `PROT_WRITE`, `PROT_EXEC`, `PROT_NONE` (can be ORed together). E.g., `PROT_READ | PROT_WRITE`.
*   `flags`: **Important:** Must use `MAP_SHARED` for changes to be visible to other processes. `MAP_PRIVATE` creates a copy-on-write mapping.
*   `fd`: File descriptor from `shm_open()`.
*   `offset`: Starting offset within the object to map (usually 0).
*   `addr`, `length` (for `munmap`): The pointer and size returned by `mmap()`.

<p align="center">
  <img src="https://github.com/user-attachments/assets/c34e8c29-047c-468b-9bdd-36a9807ce32e" width="500"/>
  <br/>
  <em>Figure 5: Table comparing Memory Mapping types. POSIX Shared Memory uses "Shared file mapping" (mapping a file-like object shared between processes).</em>
</p>

### 10.4.4. Unlink Shared Memory Object (`shm_unlink`)

Removes the object name from the system. The object is fully destroyed only when no references (maps, open fds) remain *and* `shm_unlink` has been called.

```c
#include <sys/mman.h>

int shm_unlink(const char *name);
// Returns 0 on success, -1 on error.
```

*   `name`: Name of the object (e.g., `/my_shm`).

### 10.4.5. POSIX Shared Memory Example

**shm_writer_posix.c (Writing Process):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      /* For O_* constants */
#include <sys/stat.h>   /* For mode constants */
#include <sys/mman.h>
#include <unistd.h>     /* For ftruncate, sleep, close */
#include <sys/types.h>

#define SHM_NAME "/my_posix_shm" // Object name (must start with /)
#define SHM_SIZE 1024           // Size

int main() {
    int fd;
    char *shm_ptr;
    char *data_to_write = "Hello from POSIX Writer!";

    // 1. Create/Open shared memory object (with read/write permissions)
    // O_CREAT: Create if it doesn't exist
    // O_RDWR: Open for reading and writing
    // 0660: Permissions for user and group (if created new)
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0660);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    printf("Writer: Shared memory object created/opened. FD: %d\n", fd);

    // 2. Set the size (crucial when creating new!)
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME); // Clean up on error
        exit(EXIT_FAILURE);
    }
    printf("Writer: Shared memory object size set to %d bytes.\n", SHM_SIZE);

    // 3. Map the object into the address space
    // PROT_WRITE: Need write permission
    // MAP_SHARED: Changes will be shared
    shm_ptr = (char *)mmap(NULL, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(SHM_NAME); // Clean up
        exit(EXIT_FAILURE);
    }
    printf("Writer: Shared memory mapped at address: %p\n", shm_ptr);

    // After successful mapping, the file descriptor can be closed without affecting the map
    // close(fd); // Closing fd early saves resources

    // 4. Write data
    printf("Writer: Writing data: \"%s\"\n", data_to_write);
    strncpy(shm_ptr, data_to_write, SHM_SIZE - 1);
    shm_ptr[SHM_SIZE - 1] = '\0'; // Ensure null-termination

    // Wait for reader (needs synchronization)
    printf("Writer: Data written. Waiting for reader...\n");
    sleep(10);

    // 5. Unmap the object
    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        perror("munmap");
        // exit(EXIT_FAILURE); // Should still try to unlink
    }
    printf("Writer: Shared memory unmapped.\n");

    // 6. Close the file descriptor (if not already closed)
    if (close(fd) == -1) {
        perror("close");
    }

    // 7. Unlink the object (marks for deletion)
    // Someone (usually the last process or a manager) needs to call unlink
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
        // exit(EXIT_FAILURE);
    }
    printf("Writer: Shared memory object unlinked.\n");

    exit(EXIT_SUCCESS);
}
```

**shm_reader_posix.c (Reading Process):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

#define SHM_NAME "/my_posix_shm"
#define SHM_SIZE 1024

int main() {
    int fd;
    char *shm_ptr;

    // 1. Open the existing shared memory object (read-only)
    // Note: Do not use O_CREAT here.
    fd = shm_open(SHM_NAME, O_RDONLY, 0); // mode is irrelevant when just opening
    if (fd == -1) {
        perror("shm_open (Reader might start before writer? Check name)");
        exit(EXIT_FAILURE);
    }
    printf("Reader: Shared memory object opened. FD: %d\n", fd);

    // 2. No need for ftruncate since we are opening an existing object with size

    // 3. Map the object into the address space (only need read permission)
    shm_ptr = (char *)mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Reader: Shared memory mapped at address: %p\n", shm_ptr);

    // Can close fd after mapping
    // close(fd);

    // 4. Read data
    printf("Reader: Waiting a bit before reading...\n");
    sleep(1); // Wait for writer (needs synchronization)
    printf("Reader: Data read from shared memory: \"%s\"\n", shm_ptr);

    // 5. Unmap the object
    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        perror("munmap");
        // exit(EXIT_FAILURE);
    }
    printf("Reader: Shared memory unmapped.\n");

    // 6. Close the file descriptor (if not already closed)
    if (close(fd) == -1) {
        perror("close");
    }

    // 7. The reader typically does not unlink the object.

    exit(EXIT_SUCCESS);
}
```

**How to Compile and Run:**

1.  Save the code: `shm_writer_posix.c`, `shm_reader_posix.c`.
2.  Compile (linking with the real-time library `-lrt` might be required on older Linux systems, but often not needed on modern ones):
    ```bash
    # Try without -lrt first:
    gcc shm_writer_posix.c -o writer_posix
    gcc shm_reader_posix.c -o reader_posix
    # If compilation fails with 'undefined reference' to shm_* functions, use -lrt:
    # gcc shm_writer_posix.c -o writer_posix -lrt
    # gcc shm_reader_posix.c -o reader_posix -lrt
    ```
3.  Run the writer: `./writer_posix`
4.  Run the reader in another terminal: `./reader_posix`

---

## 10.5. Important Note: Synchronization

As mentioned earlier, neither System V nor POSIX Shared Memory provides **built-in synchronization mechanisms**. If multiple processes write concurrently, or if one process writes while another reads, you can encounter **race conditions**, leading to corrupted or inconsistent data.

To ensure safe access to the shared memory region, you **MUST** use other synchronization mechanisms, such as:

*   **Semaphores (System V or POSIX):** Used to control the number of processes that can access a resource concurrently or to signal between processes.
*   **Mutexes (Typically POSIX Threads Mutexes):** Used to ensure that only one process/thread can access a critical section of code or data at a time. Mutexes can be stored *within* the shared memory region if their attributes are set to process-shared (`PTHREAD_PROCESS_SHARED`).
*   **Condition Variables:** Often used with Mutexes to wait for a specific condition to become true before proceeding.
*   **File Locking (`fcntl`):** Less common for shared memory but also an option.

Choosing and correctly implementing the appropriate synchronization mechanism is crucial when working with Shared Memory.

---
