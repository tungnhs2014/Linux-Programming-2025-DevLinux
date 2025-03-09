# Linux File System
## 🏁 2.1. Introduction to the Linux File System
### 2.1.1. Introduction
Linux follows the philosophy: **"Everything in Linux is a file."** This includes:
✅ Regular files (text files, binary files, executables)
✅ Directories (folders containing other files)
✅ Hardware devices (represented as files)
✅ Storage devices (mounted as file systems)
✅ Sockets and Pipes (inter-process communication)

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
## 🗂️ 2.3. File Management
## 🔒 2.4. File Locking in Linux
