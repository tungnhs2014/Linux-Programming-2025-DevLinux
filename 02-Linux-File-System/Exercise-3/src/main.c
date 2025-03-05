#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_PATH "file/"   // Directory where files are stored

int main(int argc, char *argv[]) 
{
    // Check if the correct number of arguments is provided
    if (argc > 5) {
        fprintf(stderr, "Error: Expected exactly 5 arguments, but received %d.\n", argc);
        return -1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s%s", FILE_PATH, argv[1]); // Construct full file path

    int num_bytes = atoi(argv[2]);  // Convert number of bytes from string to integer
    char mode = argv[3][0];         // Extract mode ('r' for read, 'w' for write)
    char *content = argv[4];        // Content to write or read (if mode is 'w' or 'r')
    
    // Read mode
    if ('r' == mode) {
        int fd = open(filepath, O_RDONLY);
        if (-1 == fd) {
            fprintf(stderr, "Error opening file for reading");
            return -1;
        }
        
        char buffer[num_bytes + 1];
        int bytes_read = read(fd, buffer, num_bytes);
        if (-1 == bytes_read) {
            fprintf(stderr, "Error reading file");
            close(fd);
            return -1;
        }

        buffer[bytes_read] = '\0';  // Null-terminate string
        printf("Read from file: %s\n", buffer);

        close(fd);
    } 
    // Write mode
    else if ('w' == mode) {
        /*  Open file for writing
            O_WRONLY: Open file in write-only mode
            O_CREAT: Create the file if it does not exist
            O_TRUNC: Truncate the file to zero length if it already exists
            S_IRUSR | S_IWUSR: Set file permissions to allow read/write access for the owner 
        */
        int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (-1 == fd) {
            fprintf(stderr, "Error opening file for writing");
            return -1;
        }
        
        int bytes_write = write(fd, content, num_bytes);
        if (-1 == bytes_write) {
            fprintf(stderr, "Error writing to file");
            close(fd);
            return -1;
        }

        printf("Written %ld to file: %s\n", strlen(content), content);

        close(fd);
    }
    // Invalid mode 
    else {
        fprintf(stderr, "Invalid mode. Use 'r' for read or 'w' for write.\n");
        return -1;
    }
    
    return 0;
}
