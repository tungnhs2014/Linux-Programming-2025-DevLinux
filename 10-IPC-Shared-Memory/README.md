# 10. IPC - Shared Memory

## 10.1. Introduction

### 10.1.1. What is IPC?

IPC (Inter-Process Communication) refers to mechanisms that allow separate processes to communicate and exchange data within an operating system.

The main aspects of IPC include:

* **Data Transfer/Sharing:** Enables one process to send information to other processes.
* **Synchronization:** Ensures processes access shared resources in an orderly manner, preventing race conditions.

**Examples of common IPC mechanisms:** Pipes, FIFOs, Message Queues, Semaphores, Shared Memory, Sockets.

<p align="center">
  <img src="https://github.com/user-attachments/assets/3c5fb086-13c9-4faa-8a4c-cade552614a9" alt="IPC Mechanisms Diagram" width="70%"/>
  <br/>
  <em>Figure 1: Model of Inter-Process Communication (IPC)</em>
</p>

### 10.1.2. What is Shared Memory?

Shared Memory is an efficient IPC mechanism that allows multiple processes to **directly access the same region of physical memory (RAM)**. The kernel allocates a memory segment and maps it into the virtual address space of each requesting process.

<p align="center">
  <img src="https://github.com/user-attachments/assets/3125df9c-68b6-40ec-b170-1964834de116" alt="Process Virtual Address Space Diagram" width="70%"/>
  <br/>
  <em>Figure 2: Illustration of a process's virtual address space. The Shared Memory/Mapped region is typically located in the higher part of the address space, separate from the Stack and Heap.</em>
</p>

When one process writes data to the shared memory region, other processes with access to this region can immediately see the changes **without any system calls to copy data**.

There are two main API standards for working with shared memory on Unix-like systems:

1. **System V Shared Memory:** An older standard, part of the System V IPC suite. Uses functions like `shmget`, `shmat`, `shmdt`, `shmctl`.
2. **POSIX Shared Memory:** A newer, more flexible standard. Uses functions like `shm_open`, `ftruncate`, `mmap`, `munmap`, `shm_unlink`.

--- 

## 10.2. Why Use Shared Memory?

Shared Memory is the preferred IPC mechanism when **data exchange speed** is critical.

* **Superior Speed:** It is the **fastest** IPC method for transferring large amounts of data between processes. Since processes access the same physical memory directly, data doesn't need to be copied.
* **Resource Efficiency:** Minimizes CPU and memory overhead associated with data copying.
* **Flexibility:** Allows complex data structures to be built directly in the shared memory region for multiple processes to use.

**However, Shared Memory also has drawbacks:**

* **Complex Synchronization:** Shared Memory **does not provide built-in synchronization mechanisms**. Processes must manage concurrent access using other IPC tools like Semaphores or Mutexes to avoid race conditions.
* **Management:** Careful management of shared memory objects is required to prevent resource leaks.

---

## 10.3. System V Shared Memory

System V Shared Memory manages memory regions using system-wide unique integer identifiers.

**Key Implementation Steps:**

1. **Create or Get Key:** Use `ftok()` to generate a unique `key_t` based on a file path and project ID.
2. **Create or Get Segment ID:** Use `shmget()` with the key to request a new shared memory segment or get an existing one.
3. **Attach Segment:** Use `shmat()` to map the segment into the process's address space.
4. **Use Segment:** Read/write data using the pointer from `shmat()`. **(External synchronization required!)**
5. **Detach Segment:** Use `shmdt()` to unmap the segment from the process's address space.
6. **Control and Release Segment:** Use `shmctl()` with commands like `IPC_RMID` to remove the segment.

### 10.3.1. Create Key (`ftok`)

The `ftok()` function generates a key based on a file path and project ID. Different processes can generate the same key if they use identical parameters.

```c
#include <sys/ipc.h>
#include <sys/shm.h>

key_t ftok(const char *pathname, int proj_id);
// Returns key_t on success, (key_t)-1 on error.
```

* `pathname`: Path to an existing file
* `proj_id`: A non-zero integer (typically uses only the least significant 8 bits)

### 10.3.2. Create/Get Shared Memory Segment (`shmget`)

The `shmget()` function creates a new segment or gets the ID of an existing one.

```c
#include <sys/ipc.h>
#include <sys/shm.h>

int shmget(key_t key, size_t size, int shmflg);
// Returns shmid (segment identifier) on success, -1 on error.
```

* `key`: The `key_t` from `ftok()` or `IPC_PRIVATE` for private segments.
* `size`: The size (in bytes) of the segment. Can be 0 when getting an existing segment.
* `shmflg`: Control flags and access permissions:
  * `IPC_CREAT`: Create the segment if it doesn't exist.
  * `IPC_EXCL`: Used with `IPC_CREAT`, fails if the segment already exists.
  * Access Permissions (e.g., `0666` for read/write by user, group, others):
    | Bit  | Meaning          |
    |------|------------------|
    | 0400 | Read by user     |
    | 0200 | Write by user    |
    | 0040 | Read by group    |
    | 0020 | Write by group   |
    | 0004 | Read by others   |
    | 0002 | Write by others  |

### 10.3.3. Attach/Detach Shared Memory Segment (`shmat`, `shmdt`)

`shmat()` maps the segment into the process's address space, and `shmdt()` unmaps it.

```c
#include <sys/types.h>
#include <sys/shm.h>

void *shmat(int shmid, const void *shmaddr, int shmflg);
// Returns pointer to memory region on success, (void *)-1 on error.

int shmdt(const void *shmaddr);
// Returns 0 on success, -1 on error.
```

* `shmid`: Segment ID from `shmget()`.
* `shmaddr` (for `shmat`): Suggested address. Usually `NULL` to let the system choose.
* `shmflg` (for `shmat`): Flags like `SHM_RDONLY` for read-only access.
* `shmaddr` (for `shmdt`): The pointer returned by `shmat()`.

<p align="center">
  <img src="https://github.com/user-attachments/assets/f587682f-4116-4056-a16b-c0e866d77a53" width="70%"/>
  <br/>
  <em>Figure 3: Illustration of `shmat` mapping a segment into a process's address space.</em>
</p>

Note that `shmdt()` only detaches the segment from the current process; it doesn't remove it from the system.

### 10.3.4. Control and Release Shared Memory Segment (`shmctl`)

`shmctl()` performs control operations on segments.

```c
#include <sys/ipc.h>
#include <sys/shm.h>

int shmctl(int shmid, int cmd, struct shmid_ds *buf);
// Returns 0 on success, -1 on error.
```

* `shmid`: Segment ID.
* `cmd`: Command to execute:
  * `IPC_STAT`: Get segment information, stored in `buf`.
  * `IPC_SET`: Set permissions and attributes from `buf`.
  * `IPC_RMID`: Mark the segment for removal when all processes detach.
* `buf`: Pointer to a `shmid_ds` structure for information (can be NULL for `IPC_RMID`).

```c
// Structure containing segment information 
struct shmid_ds {
    struct ipc_perm shm_perm;  /* Access permissions (owner, group, mode) */
    size_t          shm_segsz; /* Size of segment (bytes) */
    pid_t           shm_lpid;  /* PID of last process performing operation */
    pid_t           shm_cpid;  /* PID of creator */
    shmatt_t        shm_nattch;/* Number of current attaches */
    time_t          shm_atime; /* Last attach time */
    time_t          shm_dtime; /* Last detach time */
    time_t          shm_ctime; /* Last change time */
    // ...
};

// Structure for permissions
struct ipc_perm {
    uid_t    uid;    /* Owner's user ID */
    gid_t    gid;    /* Owner's group ID */
    uid_t    cuid;   /* Creator's user ID */
    gid_t    cgid;   /* Creator's group ID */
    mode_t   mode;   /* Access modes */
    // ...
};
```

<p align="center">
  <img src="https://github.com/user-attachments/assets/c34e8c29-047c-468b-9bdd-36a9807ce32e" width="70%"/>
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
    // When just getting the ID, size must match or be 0
    shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget (Reader might start before writer? Check key/perms)");
        exit(EXIT_FAILURE);
    }
    printf("Reader: Shared memory segment ID: %d\n", shmid);

    // 3. Attach the segment
    shm_ptr = (char *)shmat(shmid, NULL, 0); // Attach for read/write
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

**Improved Synchronization Example**

While the above examples work for demonstration, real-world applications need proper synchronization. Here's an example using a simple flag in shared memory:

```c
// Structure for shared data with synchronization
struct shared_data {
    int ready_flag;        // 0 = not ready, 1 = data ready, 2 = data read
    char message[1020];    // Actual data (adjusted for struct alignment)
};

// In writer:
struct shared_data *data = (struct shared_data *)shm_ptr;
data->ready_flag = 0;  // Initialize as not ready
strcpy(data->message, "Hello with synchronization!");
data->ready_flag = 1;  // Signal data is ready
printf("Writer: Data ready, waiting for reader to process...\n");

// Wait for reader to signal data was read
while (data->ready_flag != 2) {
    usleep(10000);  // Sleep for 10ms to reduce CPU usage
}
printf("Writer: Reader has processed the data.\n");

// In reader:
struct shared_data *data = (struct shared_data *)shm_ptr;
// Wait for data to be ready
while (data->ready_flag != 1) {
    usleep(10000);  // Sleep for 10ms
}
printf("Reader: Data received: \"%s\"\n", data->message);
data->ready_flag = 2;  // Signal data was read
```

**How to Compile and Run:**

1. Save the code into two files: `shm_writer_sysv.c` and `shm_reader_sysv.c`.
2. Ensure that `/tmp` exists (it usually does on most systems).
3. Compile:
    ```bash
    gcc shm_writer_sysv.c -o writer_sysv
    gcc shm_reader_sysv.c -o reader_sysv
    ```
4. Run the writer in one terminal: `./writer_sysv`
5. Run the reader in another terminal (within 10 seconds): `./reader_sysv`

---

## 10.4. POSIX Shared Memory

POSIX Shared Memory uses a model based on file descriptors and memory mapping (`mmap`). Shared memory objects are identified by names (like filenames, typically starting with `/`).

**Key Implementation Steps:**

1. **Create/Open Shared Memory Object:** Use `shm_open()` with a unique name to get a file descriptor.
2. **Set Size:** For new objects, use `ftruncate()` to allocate space.
3. **Map Object:** Use `mmap()` to map the object into the process's address space.
4. **Use Memory Region:** Read/write data via the returned pointer. **(External synchronization required!)**
5. **Unmap Object:** Use `munmap()` to unmap the region.
6. **Close File Descriptor:** Use `close()` to close the file descriptor.
7. **Unlink Object:** Use `shm_unlink()` to remove the object's name from the system.

### 10.4.1. Create/Open Shared Memory Object (`shm_open`)

```c
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */

int shm_open(const char *name, int oflag, mode_t mode);
// Returns file descriptor on success, -1 on error.
```

* `name`: Name of the object, must start with `/` (e.g., `/my_shm`).
* `oflag`: File opening flags:
  * `O_RDONLY`: Open for reading only.
  * `O_RDWR`: Open for reading and writing.
  * `O_CREAT`: Create the object if it doesn't exist.
  * `O_EXCL`: With `O_CREAT`, fails if the object already exists.
  * `O_TRUNC`: If the object exists, truncate it to zero length.
* `mode`: Access permissions if creating a new object (e.g., `0660`).

### 10.4.2. Set Size (`ftruncate`)

```c
#include <unistd.h>
#include <sys/types.h>

int ftruncate(int fd, off_t length);
// Returns 0 on success, -1 on error.
```

* `fd`: File descriptor from `shm_open()`.
* `length`: Desired size in bytes.

### 10.4.3. Map/Unmap Shared Memory Object (`mmap`, `munmap`)

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
// Returns pointer to mapped area on success, MAP_FAILED ((void *)-1) on error.

int munmap(void *addr, size_t length);
// Returns 0 on success, -1 on error.
```

* `addr`: Suggested starting address (usually `NULL`).
* `length`: Size of the region to map.
* `prot`: Memory protection flags:
  * `PROT_READ`: Region can be read.
  * `PROT_WRITE`: Region can be written.
  * `PROT_EXEC`: Region can execute.
  * `PROT_NONE`: Region cannot be accessed.
* `flags`: **Important options:**
  * `MAP_SHARED`: Updates are visible to other processes and written to the underlying object.
  * `MAP_PRIVATE`: Creates a private copy-on-write mapping.
* `fd`: File descriptor from `shm_open()`.
* `offset`: Starting offset within the object (usually 0).

<p align="center">
  <img src="https://github.com/user-attachments/assets/c34e8c29-047c-468b-9bdd-36a9807ce32e" width="70%"/>
  <br/>
  <em>Figure 5: Table comparing Memory Mapping types. POSIX Shared Memory uses "Shared file mapping" (mapping a file-like object shared between processes).</em>
</p>

### 10.4.4. Unlink Shared Memory Object (`shm_unlink`)

```c
#include <sys/mman.h>

int shm_unlink(const char *name);
// Returns 0 on success, -1 on error.
```

* `name`: Name of the object (e.g., `/my_shm`).

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
    shm_ptr = (char *)mmap(NULL, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(SHM_NAME); // Clean up
        exit(EXIT_FAILURE);
    }
    printf("Writer: Shared memory mapped at address: %p\n", shm_ptr);

    // After successful mapping, the file descriptor can be closed
    // without affecting the mapping
    // close(fd); // Uncomment to close fd early (saves resources)

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
    fd = shm_open(SHM_NAME, O_RDONLY, 0); // mode is irrelevant when just opening
    if (fd == -1) {
        perror("shm_open (Reader might start before writer? Check name)");
        exit(EXIT_FAILURE);
    }
    printf("Reader: Shared memory object opened. FD: %d\n", fd);

    // 2. No need for ftruncate since we're opening an existing object

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

    // 7. The reader typically does not unlink the object

    exit(EXIT_SUCCESS);
}
```

**Improved POSIX Synchronization Example**

Here's an enhanced example using a structured data approach with synchronization:

```c
// Structure for shared data with synchronization flags
struct shared_data {
    int status;          // 0=empty, 1=filled, 2=processed
    int counter;         // Message counter for tracking
    char message[1016];  // Actual message content
};

// In the writer:
// Map the memory
struct shared_data *data = (struct shared_data *)shm_ptr;

// Initialize the shared data
data->status = 0;  // Empty
data->counter = 0; // Start counter at 0

// Prepare and send multiple messages
for (int i = 1; i <= 5; i++) {
    // Wait until reader has processed previous message
    while (data->status == 1) {
        usleep(10000);  // 10ms pause
    }
    
    // Fill the shared memory with new data
    data->counter = i;
    snprintf(data->message, sizeof(data->message), 
             "Message #%d from POSIX writer", i);
    
    // Signal that data is ready for reading
    data->status = 1;
    
    printf("Writer: Sent message %d\n", i);
    
    // Small delay between messages
    usleep(100000);  // 100ms
}

// In the reader:
// Map the memory
struct shared_data *data = (struct shared_data *)shm_ptr;

// Read 5 messages
for (int i = 1; i <= 5; i++) {
    // Wait until writer has filled the buffer
    while (data->status != 1) {
        usleep(10000);  // 10ms pause
    }
    
    // Process the data
    printf("Reader: Received message %d: '%s'\n", 
           data->counter, data->message);
    
    // Signal that we've processed the data
    data->status = 0;
    
    // Small delay between reads
    usleep(50000);  // 50ms
}
```

**How to Compile and Run:**

1. Save the code: `shm_writer_posix.c`, `shm_reader_posix.c`.
2. Compile:
    ```bash
    # On most modern systems:
    gcc shm_writer_posix.c -o writer_posix
    gcc shm_reader_posix.c -o reader_posix
    
    # If you get undefined references to shm_* functions, add -lrt:
    # gcc shm_writer_posix.c -o writer_posix -lrt
    # gcc shm_reader_posix.c -o reader_posix -lrt
    ```
3. Run the writer: `./writer_posix`
4. Run the reader in another terminal: `./reader_posix`