# Linux File System
## 🏁 2.1. Introduction to the Linux File System
### 2.1.1. Introduction
Linux follows the philosophy: **"Everything in Linux is a file."** This includes:
- Regular files (text files, binary files, executables)
- Directories (folders containing other files)
- Hardware devices (represented as files)
- Storage devices (mounted as file systems)
- Sockets and Pipes (inter-process communication)

To display detailed information about files and directories:
```sh
ls -l
```
Example:
```
drwxr-xr-x  2 user user 4096 Mar 09 12:00 Documents
-rw-r--r--  1 user user 1234 Mar 09 12:34 file.txt
lrwxrwxrwx  1 user user   11 Mar 09 12:40 shortcut -> /home/user/file
crw-rw-rw-  1 root root 1, 3 Mar 09 12:50 /dev/null
brw-rw----  1 root disk 8, 1 Mar 09 13:00 /dev/sda1
```

---

### 2.1.2. Types of Files in Linux
Linux categorizes files into different types:

| **File Type**         | **Symbol** | **Description** |
|----------------------|------------|----------------|
| **Regular File**     | `-`        | Normal file (text, binary, executables). |
| **Directory**        | `d`        | Folder containing files. |
| **Character Device** | `c`        | Represents character devices (keyboard, mouse). |
| **Block Device**     | `b`        | Represents block devices (hard disks, USB). |
| **Symbolic Link**    | `l`        | Shortcut to another file or directory. |
| **Socket**          | `s`        | Used for inter-process communication. |
| **Pipe**            | `p`        | Creates a pipeline between processes. |

To check the file type:
```sh
ls -l filename
file filename
```
![Image](https://github.com/user-attachments/assets/10ebb6d0-f00b-41b2-b92a-08aba9cc9e08)

---

### 2.1.3. Linux File System Structure
The Linux file system is structured as a tree, starting from the root directory `/`.

| **Directory** | **Description** |
|-------------|---------------|
| `/` | Root directory containing all other files and directories. |
| `/bin` | Essential binary executables (ls, cp, mv, cat, grep, …). |
| `/sbin` | System administration binaries (fsck, reboot, iptables). |
| `/etc` | Configuration files for the system. |
| `/home` | Home directories for users. |
| `/root` | Home directory of the root user. |
| `/dev` | Device files representing hardware components. |
| `/proc` | Virtual filesystem containing system and process info. |
| `/sys` | Information about hardware devices. |
| `/var` | Logs, cache, and mail data. |
| `/tmp` | Temporary files (deleted on reboot). |
| `/usr` | Installed applications and system libraries. |
| `/lib` | Shared libraries (*.so files). |
| `/mnt` | Mount points for external storage devices. |
| `/opt` | Optional third-party software. |

To display the directory structure:
```sh
tree /
ls /
```

---

### 2.1.4. File Permissions & Ownership
Linux enforces strict permissions for security, defined for:
- **Owner (`u`)** – The file's owner.
- **Group (`g`)** – A group of users with access.
- **Others (`o`)** – All other users.

| **Symbol** | **Value** | **Description** |
|----------|--------|--------------|
| `r` | 4 | Read permission. |
| `w` | 2 | Write permission. |
| `x` | 1 | Execute permission. |
| `-` | 0 | No permission. |

Example:
```
-rwxr--r-- 1 user user 1234 Mar 09 12:34 file.txt
```
🔹 **Owner** has **rwx** (read, write, execute).
🔹 **Group** has **r--** (read only).
🔹 **Others** have **r--** (read only).

### **🔹 Changing File Permissions (`chmod`)**
```sh
chmod 755 file.txt   # rwxr-xr-x
chmod u+w file.txt   # Add write permission for owner.
chmod g-r file.txt   # Remove read permission from group.
```

### **🔹 Changing File Ownership (`chown`)**
```sh
chown user file.txt   # Change file owner.
chown user:group file.txt   # Change both owner and group.
```

---

### 2.1.5. Disk Management & Partitioning
### **🔹 Check Disk Usage**
```sh
df -h      # Check disk space.
du -sh *   # Check directory size.
```

### **🔹 Creating and Managing Partitions**
```sh
fdisk /dev/sdb  # Manage disk partitions.
mkfs.ext4 /dev/sdb1  # Format a partition.
fsck -y /dev/sdb1  # Check for filesystem errors.
```

### **🔹 Mount and Unmount a Disk**
```sh
mount /dev/sdb1 /mnt
umount /mnt
```

---

### 2.1.6. Linux File Systems
| **File System** | **Description** |
|---------------|----------------|
| **ext4** | Default Linux file system with journaling support. |
| **XFS** | High-performance file system for large systems. |
| **Btrfs** | Supports snapshots and compression. |
| **vfat** | Compatible with Windows. |
| **ntfs** | Windows file system, readable on Linux. |
| **tmpfs** | Temporary filesystem stored in RAM. |

To check the file system type:
```sh
df -T
mount | column -t
```

---

### 2.1.7 Virtual File Systems: `/proc` and `/sys`
💡 **`/proc`** – Provides system and process information.
```sh
cat /proc/cpuinfo  # CPU information.
cat /proc/meminfo  # RAM information.
```

💡 **`/sys`** – Contains details about hardware devices.
```sh
ls /sys/class/net  # List network interfaces.
cat /sys/block/sda/size  # Check disk size.
```

---

## 📂 2.2. Operations on Files
### Linux System Calls
System calls are interfaces that allow user applications to interact directly with the kernel. They enable file manipulation, process management, memory allocation, and inter-process communication (IPC).

--- 
### 2.2.1. File Management
- `open()` - Open or create a file.  
- `read()` - Read data from a file.  
- `write()` - Write data to a file.  
- `lseek()` - Move file pointer.  
- `close()` - Close an open file.  
- `unlink()` - Delete a file.  
- `rename()` - Rename a file.  

---
### 2.2.2. Process Management
- `fork()` - Create a new process.  
- `exec()` - Replace current process image.  
- `wait()` - Wait for a child process to finish.  
- `exit()` - Terminate the current process.  
- `getpid()` - Get process ID.  
- `kill()` - Send a signal to a process.  

---
### 2.2.3. Memory Management
- `brk()` - Adjust the program break.  
- `mmap()` - Map files or devices into memory.  
- `munmap()` - Unmap a memory region.  

---

### 2.2.4. Inter-Process Communication (IPC)
- `pipe()` - Create a unidirectional data channel.  
- `shmget()` - Get shared memory segment.  
- `shmat()` - Attach shared memory segment.  
- `msgget()` - Create/access a message queue.  
- `msgsnd()` - Send a message to a queue.  
- `msgrcv()` - Receive a message from a queue.  

---

### 2.2.5. Networking
- `socket()` - Create a network socket.  
- `bind()` - Bind a socket to an address.  
- `listen()` - Listen for incoming connections.  
- `accept()` - Accept a new connection.  
- `connect()` - Connect to a remote socket.  
- `send()` / `recv()` - Send and receive data over a socket.  

---

### Usage Example
```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("example.txt", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    write(fd, "Hello, World!\n", 14);
    close(fd);
    return 0;
}
```
### Tips
- Always check the return value of system calls for error handling.
- Use perror() to print a human-readable error message.
- Make sure to close() file descriptors after use to avoid resource leaks.

---

## 🗂️ 2.3. File Management
### 2.3.1.Page Cache in Linux
- Page cache is a mechanism used by the Linux kernel to cache data from disk storage into physical memory (RAM) to improve I/O performance. It acts as a buffer between user space applications and the physical disk.

#### How Page Cache Works
- **Reading Data (from Cache or Disk)**
  + The kernel determines which page to read.
  + The kernel first checks whether the requested page is present in the page cache.
  + If the page is found in the cache (a cache hit), the data is directly read from the cache, avoiding disk I/O.
  + If the page is not found in the cache (a cache miss), the kernel reads the data from the physical disk into the page cache and then passes it to the user space.

- **Writing Data (via Cache)**
  + The kernel writes page contents to the page cache rather than directly to the physical disk.
  + Data in the page cache is written to disk periodically or explicitly using system calls like sync() or fsync().

![Image](https://github.com/user-attachments/assets/ee94175f-6695-42aa-9bbc-a9fd93bac091)

#### Why Use Page Cache?
- **Performance Improvement:** Reduces the number of I/O operations by keeping frequently accessed data in memory.
- **Reduced Latency:** Accessing RAM is significantly faster than reading from disk.
- **Efficient Disk Usage:** Batches disk writes to minimize I/O operations.

#### Syncing Page Cache to Disk
- The page cache is not immediately synchronized with the disk, which means there might be a delay between writing data and it being physically stored. To ensure data persistence, the following commands are used:
  + `sync():` Flushes all cached data to disk.
  + `fsync(fd):` Flushes the data of a specific file descriptor to disk.
    
#### Advantages of Page Cache
- Reduces disk access frequency.
- Increases system responsiveness.
- Optimizes disk read/write operations.
  
#### Disadvantages of Page Cache
- Data loss can occur if the system crashes before the cache is flushed to disk.
- High memory usage if the cache is not properly managed.

### 2.3.2. File Management in Linux
#### Overview
In Linux, the process of managing files involves multiple tables that help efficiently track and access file data. The key components involved are:
1. **I-node Table**
2. **Open File Table**
3. **File Descriptor Table**
4. **File Opening Process**

---

#### I-node Table
The I-node Table stores metadata about files and directories. Each file in the filesystem has a unique I-node that contains:
- **I-node Number:** A unique identifier for the file.
- **File Metadata:** Permissions, ownership, timestamps (creation, modification, access), file size, etc.
- **Data Block Pointers:** Pointers to the actual data blocks on the disk.

#### Structure of I-node Table:
| I-node Number | Data Block Pointers |
|--------------|---------------------|
| 142          | 234, 125, 253        |
| 1145         | 5356, 9839           |
| 5632         | 821, 2929            |

#### Key Points:
- An I-node does **not** store the filename; it only contains metadata and data pointers.
- Multiple file names (hard links) can point to the same I-node.

![Image](https://github.com/user-attachments/assets/5d0bf71a-afd6-4567-926a-0eb4e6dfdfb0)

---

#### Open File Table
The Open File Table keeps track of all files opened by the operating system. It provides information about:
- **File Offset:** The current read/write position in the file.
- **Status Flags:** The mode in which the file is opened (e.g., `O_RDWR`, `O_RDONLY`, `O_CREAT`).
- **I-node Pointer:** Points to the corresponding I-node entry.

### Structure of Open File Table:
| File Offset | Status Flags        | I-node Pointer |
|------------|--------------------|----------------|
| 0          | O_RDWR | O_CREAT    | 1145           |
| 16         | O_RDONLY            | 5632           |
| 100        | O_RDWR              | 142            |

### Key Points:
- A file opened multiple times by the same or different processes will have separate entries.
- Each entry points to the **I-node Table** for actual file data.

![Image](https://github.com/user-attachments/assets/7226d6da-7d56-46e2-a9f4-8e7c0f1c3d48)

---

#### File Descriptor Table
Each process has its own **File Descriptor Table**, which maintains references to the opened files. It contains:
- **File Descriptor (FD):** An integer used to access an opened file.
- **Fd Flags:** Flags specifying how the file was opened (e.g., `O_RDWR`, `O_RDONLY`).
- **File Pointer:** Points to the **Open File Table** entry.

### Structure of File Descriptor Table:
| File Descriptor | Fd Flags           | File Pointer |
|----------------|--------------------|--------------|
| fd0            | O_RDWR              | 100          |
| fd1            | O_RDONLY            | 16           |
| fd2            | O_RDWR | O_CREAT    | 0            |

### Key Points:
- The first three file descriptors (`fd0`, `fd1`, `fd2`) usually correspond to **Standard Input**, **Standard Output**, and **Standard Error**.
- The **File Pointer** links to the **Open File Table**.

![Image](https://github.com/user-attachments/assets/c8c186b7-e0e0-4dcb-b42c-9215260128ad)

---

#### File Opening Process
The process of opening a file in Linux involves multiple steps:
1. **System Call (open()):** The application requests to open a file using a system call.
2. **File Descriptor Allocation:** The kernel allocates a new file descriptor (FD) in the **File Descriptor Table**.
3. **Open File Table Entry:** An entry is created in the **Open File Table** to maintain the file offset and flags.
4. **I-node Lookup:** The kernel locates the file’s I-node and creates a reference in the **Open File Table**.

![Image](https://github.com/user-attachments/assets/ccd92654-3fd3-4ec2-ab34-757622a7ccd1)

#### Example Flow:
1. The process calls `open()` on `hello.txt`.
2. A new **File Descriptor (fd0)** is created and linked to an entry in the **Open File Table**.
3. The **Open File Table** entry points to the appropriate **I-node Table** entry, where file metadata and data block pointers are stored.
  
---
## 🔒 2.4. File Locking in Linux

### 2.4.1 Overview
File locking is a mechanism used to control access to a file in a multi-process or multi-threaded environment, ensuring data consistency and preventing conflicts.

---

### 2.4.2. How File Locking Works
#### Steps of File Locking Mechanism:
1. **Write the lock state to the file's I-node:**
   - When a process requests a lock, the system writes the lock state to the file's I-node.
   - This information includes:
     - **Process ID:** The process that holds the lock.
     - **Lock State:** Shared (read) or exclusive (write).
     - **Lock Range:** Byte range being locked (for `fcntl()`).

2. **Perform read/write operations on the file:**
   - If the lock is successfully acquired, the process can perform **read or write** operations depending on the lock type.
   - If unsuccessful (due to a conflicting lock from another process), the system will:
     - Return an error immediately (if using **Non-blocking** mode).
     - Wait until the file is unlocked (if using **Blocking** mode).

3. **Remove the lock state from the file's I-node:**
   - After completing the read or write operation, the process will **release the lock**.
   - The system will clear the lock state from the I-node, allowing other processes to access the file.

---

### 2.4.3. Comparison: flock() vs. fcntl()

| **Feature**     | **flock()**                                           | **fcntl()**                                                                 |
|----------------|---------------------------------------------------------|------------------------------------------------------------------------------|
| **Usage Scope** | Works on entire files, simple and easy to use           | Can lock entire files or byte ranges (fine-grained control)                   |
| **Lock Types**  | Shared (read) or Exclusive (write) locks                | Shared (read), Exclusive (write), Advisory, or Mandatory locks                |
| **Scope**       | Entire file locking only                               | Supports locking specific byte ranges (record locking)                        |
| **Concurrency** | Can be used across multiple processes, simple but limited | Supports both intra-process and inter-process locking, providing more control |
| **Blocking**    | Supports Non-blocking using `LOCK_NB`                   | Supports Non-blocking with `F_SETLK`, Blocking with `F_SETLKW`                 |
| **Complexity**  | Simple and straightforward                             | More complex but flexible                                                    |

---
#### 2.4.4. flock() Function
#### Syntax:
```c
#include <sys/file.h>

int flock(int fd, int operation);
```
- **fd:** File descriptor of the file to be locked.
- **operation:** The operation to perform (lock/unlock).

#### Flags (operation):

| Flag      | Description                                                                              |
|----------|------------------------------------------------------------------------------------------|
| LOCK_SH  | Shared lock for reading. Other processes can read but not write.                           |
| LOCK_EX  | Exclusive lock for writing. No other reads or writes are allowed.                          |
| LOCK_UN  | Unlock the file.                                                                          |
| LOCK_NB  | Non-blocking: If the lock cannot be acquired, the function returns immediately with an error.|

#### Example Usage of flock():
```c
#include <stdio.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("example.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        perror("flock");
        close(fd);
        return 1;
    }

    printf("File locked exclusively!\n");
    sleep(10);  // Hold the lock for 10 seconds

    flock(fd, LOCK_UN);
    printf("File unlocked.\n");

    close(fd);
    return 0;
}
```

---

#### 2.4.5. fcntl() Function

#### Syntax:
```c
#include <fcntl.h>
#include <unistd.h>

int fcntl(int fd, int cmd, struct flock *lock);
```
- **fd:** File descriptor of the file to be locked.
- **cmd:** The command to perform (set, release, or get lock).
- **lock:** Pointer to a `struct flock` containing lock information.

#### flock Structure:
```c
struct flock {
    short l_type;    // Type of lock: F_RDLCK, F_WRLCK, F_UNLCK
    short l_whence;  // Starting point: SEEK_SET, SEEK_CUR, SEEK_END
    off_t l_start;   // Offset to start the lock
    off_t l_len;     // Length of the locked region (0 = entire file)
    pid_t l_pid;     // Process ID of the lock holder
};
```

#### Flags (cmd):

| Flag      | Description                                       |
|----------|---------------------------------------------------|
| F_SETLK  | Set or release a lock without waiting (non-blocking). |
| F_SETLKW | Set or release a lock and wait (blocking).         |
| F_GETLK  | Get information about a lock.                     |

#### Lock Types (l_type):

| Flag      | Description                     |
|----------|---------------------------------|
| F_RDLCK  | Set a shared (read) lock.         |
| F_WRLCK  | Set an exclusive (write) lock.    |
| F_UNLCK  | Release the lock.                 |

#### Example Usage of fcntl():
```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("example.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl");
        close(fd);
        return 1;
    }

    printf("File locked for writing!\n");
    sleep(10);

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    printf("File unlocked.\n");

    close(fd);
    return 0;
}
```
---
