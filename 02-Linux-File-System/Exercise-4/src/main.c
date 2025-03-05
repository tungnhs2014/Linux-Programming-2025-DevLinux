#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

int main() {
    // Define the file name to be created
    char *file_name = "file/test.txt";

    // Open the file with write permission
    // O_WRONLY: Open for writing only
    // O_CREAT: Create file if it does not exist
    // O_TRUNC: Truncate file to zero length if it already exists
    // 0644: File permissions (owner: read/write, group/others: read-only)
    int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (-1 == fd) {
        fprintf(stderr, "Error creating file");
        return -1;
    }
    
    // Define the content to write into the file
    char buffer[] = "Hello, I'm Son Tung\n";
    
    // Write data to the file
    int byte_write = write(fd, buffer, strlen(buffer));
    if(-1 == byte_write){
        fprintf(stderr, "Error writing to file");
        close(fd);
        return -1;
    }

    // Declare a struct stat variable to store file information
    struct stat fileStat;

    // Get file metadata using stat() system call
    if (stat(file_name, &fileStat) < 0) {
        fprintf(stderr, "Error getting file information");
        return -1;
    }
    
    // Print file information
    printf("File Name: %s\n", file_name);
    printf("File Size: %ld bytes\n", fileStat.st_size);
    printf("I-node number: %ju\n", (uintmax_t)fileStat.st_ino);
    printf("Last status change:     %s", ctime(&fileStat.st_ctime));
    printf("Last file access:       %s", ctime(&fileStat.st_atime));
    printf("Last file modification: %s", ctime(&fileStat.st_mtime));
    
    // Determine and print the type of file
    printf("File Type: ");
    switch (fileStat.st_mode & S_IFMT) {
        case S_IFBLK:   printf("Block device\n");       break;
        case S_IFCHR:   printf("Character device\n");   break;
        case S_IFDIR:   printf("Directory\n");          break;
        case S_IFIFO:   printf("FIFO/pipe\n");          break;
        case S_IFLNK:   printf("Symbolic link\n");      break;
        case S_IFREG:   printf("Regular file\n");       break;
        case S_IFSOCK:  printf("Socket\n");             break;
        default:        printf("Unknown\n");
    }
    
    close(fd);

    return 0;
}
