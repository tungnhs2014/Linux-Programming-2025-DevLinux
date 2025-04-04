# IPC - Pipes and FIFOs

## 8.1. Introduction to Pipes
### 8.1.1. Definition
- Pipes are an Inter-Process Communication (IPC) mechanism used for communication between related processes in Linux.
- Pipes are unidirectional, meaning data flows in only one direction: one process writes to the pipe while another reads from it.
- They provide a simple yet effective method for parent-child process communication.

<p align="center">
  <img src="https://github.com/user-attachments/assets/9fa07748-8abc-467c-ac85-276cc8aa9446" alt="Image" width="400"/>
</p>

### 8.1.2. Pipe Operation
- When a pipe is created, it exists in RAM as a **"virtual file"** with a fixed buffer size (typically 65536 bytes in modern Linux systems).
- Pipes can be created before forking processes to establish communication channels between **parent and child processes**.
- Data written to a pipe by one process can be read by another related process.
- Real-world analogy: Like pouring water (data) into a physical pipe that flows into a container, where someone can collect it with a cup. The output of the pipe becomes the input for the recipient.

<p align="center">
  <img src="https://github.com/user-attachments/assets/5f3ae4ba-57a5-4714-b8a6-9ff6ef94b293" alt="Image" width="400"/>
</p>


#### 8.1.2.1. Reading from a pipe
- When attempting to read from an empty pipe, the reading process blocks until at least one byte becomes available.
- If all write ends of a pipe are closed, a read operation will return all remaining data and then return 0 (EOF).
- This ensures processes synchronize naturally when exchanging data.

#### 8.1.2.2. Pipes have a limited capacity
- Pipes have a limited capacity (buffer size).
- When a pipe is full, the writing process blocks until some data is read from the pipe.
- If all read ends of a pipe are closed, a write operation will cause the writing process to receive a SIGPIPE signal, which by default terminates the process.

--- 

## 8.2. Creating and Using Pipes
### 8.2.1. Creating Pipes
- Pipes are created using the `pipe()`system call.
- To enable communication between parent and child processes, the pipe must be created before calling `fork()`.

```c
#include <unistd.h>

int pipe(int fds[2]);
```
- The function creates a new pipe and returns two file descriptors in the `fds` array:
  + `fds[0]`: The read end of the pipe.
  + `fds[1]`:	The write end of the pipe.
- Returns 0 on success, -1 on failure (setting errno accordingly).

#### Example of pipe creation and usage
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipe_fd[2];
    pid_t pid;
    char buffer[100];
    char message[] = "Hello from parent process!";
    
    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Create child process
    pid = fork();
    
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0) {  // Parent process
        // Close unused read end
        close(pipe_fd[0]);
        
        // Write message to pipe
        printf("Parent writing to pipe: %s\n", message);
        write(pipe_fd[1], message, strlen(message) + 1);
        
        // Close write end
        close(pipe_fd[1]);
        
        // Wait for child to complete
        wait(NULL);
    } else {  // Child process
        // Close unused write end
        close(pipe_fd[1]);
        
        // Read message from pipe
        read(pipe_fd[0], buffer, sizeof(buffer));
        printf("Child read from pipe: %s\n", buffer);
        
        // Close read end
        close(pipe_fd[0]);
    }
    
    return 0;
}
```

### 8.2.2. Bidirectional Communication
- A single pipe allows communication in only one direction.
- For bidirectional communication, two pipes are needed - one for each direction.
- When implementing bidirectional communication, care must be taken to avoid deadlock situations where both processes are blocked waiting for each other.

<p align="center">
  <img src="https://github.com/user-attachments/assets/e6991e17-1fcc-44bc-b451-35014ac77f9b" alt="Two-way Pipe Communication" width="500"/>
</p>

- Parent plays the rolo of `Writer`.
- Child plays the role of `Reader`.

<p align="center">
  <img src="https://github.com/user-attachments/assets/fa51ed86-96d3-49ca-abb2-f1c676725b36" alt="Parent as Writer - Child as Reader" width="500"/>
</p>

- Parent plays the role of `Reader`.
- Child plays the role of `Writer`.

<p align="center">
  <img src="https://github.com/user-attachments/assets/f0b83f9c-6666-4e8c-9d0e-b9407c6c3608" alt="Parent as Reader - Child as Writer" width="500"/>
</p>

### 8.2.3. Important Considerations for Pipe Usage
- To avoid resource leaks, always close unused pipe ends immediately after fork().
- Resources are only fully freed when all file descriptors referring to the pipe are closed.
- To prevent deadlocks, ensure a clear communication protocol between processes.
- Check return values of read() and write() to handle potential errors.

--- 

## 8.3. FIFOs – Named Pipes
### 8.3.1. Definition
- FIFOs (First In, First Out) or named pipes extend the concept of pipes by providing a named entity in the filesystem.
- Unlike anonymous pipes, FIFOs persist in the filesystem and can be used for communication between unrelated processes.
- FIFOs exist independently of process lifetimes and can be explicitly deleted when no longer needed.
- A FIFO appears as a special file in the filesystem, created using the `mkfifo()` function or the mkfifo command.

### 8.3.2. Creating FIFOs from the Shell
- Command syntax: `mkfifo [-m mode] pathname`
- Example: mkfifo -m 0666 ./myfifo
- Note the `'p'` at the beginning of the permissions string, indicating it's a pipe file.

<p align="center">
  <img src="https://github.com/user-attachments/assets/ec3a4ff3-ec47-48e1-8b30-52d6fd4eb481" alt="mkfifo Command Example" width="500"/>
</p>

### 8.3.3. Creating FIFOs from Source Code
- FIFOs can be programmatically created using the `mkfifo()` function
```c
#include <sys/types.h>
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);
```
- Parameters:
  + `pathname:` The name and path for the FIFO file
  + `mode:` Permission bits for the FIFO (similar to file permissions)
- Returns 0 on success, -1 on failure

#### Example of creating and using a FIFO:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// Writer program
int main() {
    int fd;
    char message[] = "Hello through FIFO!";
    
    // Create the FIFO if it doesn't exist
    mkfifo("./myfifo", 0666);
    
    printf("Opening FIFO for writing...\n");
    fd = open("./myfifo", O_WRONLY);
    
    printf("Writing message: %s\n", message);
    write(fd, message, strlen(message) + 1);
    
    close(fd);
    printf("Writer closed\n");
    
    return 0;
}
```
- And the reader program:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// Reader program
int main() {
    int fd;
    char buffer[100];
    
    // Open the FIFO for reading
    printf("Opening FIFO for reading...\n");
    fd = open("./myfifo", O_RDONLY);
    
    // Read from FIFO
    read(fd, buffer, sizeof(buffer));
    printf("Message received: %s\n", buffer);
    
    close(fd);
    printf("Reader closed\n");
    
    return 0;
}
```

### 8.3.4. FIFO Access and I/O Modes
- By default, opening a FIFO blocks until another process opens the same FIFO for the opposite access mode.
- Opening a FIFO for reading blocks until some process opens it for writing, and vice versa.
- This behavior can be modified by using the O_NONBLOCK flag with open().

---

## 8.4. Building a Client-Server Model with FIFOs
### 8.4.1. Establishing Connections
To create a client-server architecture using FIFOs:
1. Well-known address approach:
   - The server creates a well-known FIFO with a predefined name and location.
   - Clients know this name/location and can connect to the server through it.
2. Server identification:
   - Server provides multiple services.
   - Clients must request specific services when connecting.
   - This may require additional programming and overhead.

### 8.4.2. Handling Connection Requests 
#### Server Implementation
1. Create a well-known FIFO for handling client requests.
2. Process client requests by:
  - Validating authentication (if required).
  - Checking requested service availability.
  - Managing connection limits.
3. Send acceptance/rejection acknowledgment to clients.

#### Client Implementation
1. Create a client-specific FIFO for receiving responses (typically using a pattern like `client_fifo_<PID>`)
2. Prepare and send a connection request containing:
  - Client identification (name/PID).
  - Requested service.
  - Authentication key (if required)
3. Process server response:
  - If accepted, begin data exchange.
  - If rejected, clean up the client-specific FIFO and exit
#### Example server implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_FIFO "/tmp/server_fifo"
#define CLIENT_FIFO_TEMPLATE "/tmp/client_%d_fifo"
#define MAX_CLIENTS 10

typedef struct {
    pid_t client_pid;
    char service[32];
    char key[16];
} request_t;

typedef struct {
    int status; // 0 = rejected, 1 = accepted
    char message[64];
} response_t;

int main() {
    int server_fd, client_fd;
    request_t request;
    response_t response;
    char client_fifo[64];
    int connected_clients = 0;
    
    // Create server FIFO
    mkfifo(SERVER_FIFO, 0666);
    
    // Open server FIFO for reading requests
    server_fd = open(SERVER_FIFO, O_RDONLY);
    if (server_fd == -1) {
        perror("open server fifo");
        exit(EXIT_FAILURE);
    }
    
    printf("Server started. Waiting for client requests...\n");
    
    while (1) {
        // Read client request
        if (read(server_fd, &request, sizeof(request_t)) == sizeof(request_t)) {
            printf("Request from client PID %d for service: %s\n", 
                   request.client_pid, request.service);
            
            // Check if we can accept this client
            if (connected_clients < MAX_CLIENTS && strcmp(request.key, "valid_key") == 0) {
                response.status = 1;
                strcpy(response.message, "Connection accepted");
                connected_clients++;
            } else {
                response.status = 0;
                strcpy(response.message, "Connection rejected");
            }
            
            // Send response to client-specific FIFO
            sprintf(client_fifo, CLIENT_FIFO_TEMPLATE, request.client_pid);
            client_fd = open(client_fifo, O_WRONLY);
            
            if (client_fd != -1) {
                write(client_fd, &response, sizeof(response_t));
                close(client_fd);
                
                printf("Response sent to client %d: %s\n", 
                       request.client_pid, response.message);
            }
        }
    }
    
    // Cleanup
    close(server_fd);
    unlink(SERVER_FIFO);
    
    return 0;
}
```

#### Example client implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_FIFO "/tmp/server_fifo"
#define CLIENT_FIFO_TEMPLATE "/tmp/client_%d_fifo"

typedef struct {
    pid_t client_pid;
    char service[32];
    char key[16];
} request_t;

typedef struct {
    int status; // 0 = rejected, 1 = accepted
    char message[64];
} response_t;

int main() {
    int server_fd, client_fd;
    request_t request;
    response_t response;
    char client_fifo[64];
    
    // Create client-specific FIFO
    sprintf(client_fifo, CLIENT_FIFO_TEMPLATE, getpid());
    mkfifo(client_fifo, 0666);
    
    // Prepare request
    request.client_pid = getpid();
    strcpy(request.service, "data_service");
    strcpy(request.key, "valid_key");
    
    // Send request to server
    server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1) {
        perror("Cannot open server FIFO");
        unlink(client_fifo);
        exit(EXIT_FAILURE);
    }
    
    printf("Sending connection request to server...\n");
    write(server_fd, &request, sizeof(request_t));
    close(server_fd);
    
    // Wait for server response
    printf("Waiting for server response...\n");
    client_fd = open(client_fifo, O_RDONLY);
    
    if (read(client_fd, &response, sizeof(response_t)) == sizeof(response_t)) {
        printf("Server response: %s\n", response.message);
        
        if (response.status == 1) {
            printf("Connection established! Ready for data exchange.\n");
            // Data exchange would happen here
        } else {
            printf("Connection rejected by server.\n");
        }
    }
    
    // Cleanup
    close(client_fd);
    unlink(client_fifo);
    
    return 0;
}
```

### 8.4.3. Message Handling in FIFO Communication
- Since data in pipes and FIFOs is just a byte stream with no inherent message boundaries, applications must implement their own message framing protocols. There are several common approaches:
#### 8.4.3.1. Delimiter-Based Messages
- End each message with a special character that never appears in the message content.
- The reader must scan the byte stream for the delimiter.
- Example: Using newline ('\n') as a delimiter
```c
// Writer
char message[] = "Hello from client\n";
write(fifo_fd, message, strlen(message));

// Reader
char buffer[1024];
int i = 0;
char c;

while (read(fifo_fd, &c, 1) > 0) {
    if (c == '\n') {
        buffer[i] = '\0';
        // Process complete message in buffer
        i = 0;
    } else {
        buffer[i++] = c;
    }
}
```
#### 8.4.3.2. Length-Prefixed Messages
- Include a fixed-size header containing the message length.
- More efficient for arbitrary-sized messages.
- More complex to implement.
```c
// Message structure
typedef struct {
    uint32_t length;  // 4 bytes for message length
    char data[];      // Flexible array member
} message_t;

// Writer
uint32_t msg_len = strlen(message_content);
message_t *msg = malloc(sizeof(message_t) + msg_len);
msg->length = msg_len;
memcpy(msg->data, message_content, msg_len);
write(fifo_fd, msg, sizeof(message_t) + msg_len);
free(msg);

// Reader
message_t header;
read(fifo_fd, &header, sizeof(message_t));
char *buffer = malloc(header.length + 1);
read(fifo_fd, buffer, header.length);
buffer[header.length] = '\0';
// Process message in buffer
free(buffer);
```

#### 8.4.3.3. Fixed-Length Messages
- Use messages of a predetermined, fixed size.
- Simple to implement but potentially wastes bandwidth.
- Risk of message misalignment if one message is incorrect.
```c
#define MSG_SIZE 256

// Writer
char message[MSG_SIZE] = {0};
strcpy(message, "Hello from client");
write(fifo_fd, message, MSG_SIZE);

// Reader
char buffer[MSG_SIZE];
read(fifo_fd, buffer, MSG_SIZE);
// Process message in buffer
```

