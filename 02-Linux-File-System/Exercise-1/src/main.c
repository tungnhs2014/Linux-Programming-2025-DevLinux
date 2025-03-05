#include <stdio.h>
#include <fcntl.h>      // File control options (open, O_APPEND, O_CREAT)
#include <unistd.h>     // System calls
#include <sys/stat.h>   // File permissions (mkdir)        
#include <string.h>

int main(int argc, char const *argv[])
{
    char buf[20] = "Linux Programming ";
    int fd, ls;
    int number_write;

    // Create the directory "file" with permissions 0777 (full access)
    if(mkdir("file", 0777)){
        printf("ERROR!! mkdir failed");
        return -1;
    }
    /*
        Open (or create) the file "file/linux.txt" with read/write permissions
        O_CREAT: Create the file if it does not exist
        O_APPEND: Always append data to the end of the file
        0666: Set read/write permissions for all users
    */
    fd = open("file/linux.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
    if(-1 == fd){
        printf("ERROR!! Open() is failed\n");
        return -1;
    }
    
    // Move the file pointer to the beginning
    // However, since the file is opened with O_APPEND, this call has no effect
    ls = lseek(fd, 0, SEEK_SET);
    if(-1 == ls){
        printf("ERROR!! lseek\n");
        return -1;
    }

    // Write to the file (data will still be appended at the end)
    number_write = write(fd, buf, strlen(buf));
    printf("Write %d bytes to linux.txt\n", number_write);
    
    close(fd);

    return 0;
}
