# IPC Socket

## 6.1. Introduction to Socket Communication
### 6.1.1. Concept
- A socket is a communication mechanism that allows processes to exchange data, whether they're running on the same device or on different machines across a network. Sockets provide a standardized interface for interprocess communication that works across various platforms and network protocols.
- In Linux, each socket is represented by a file descriptor, similar to how files and pipes are handled. The socket descriptor contains essential information about the connection, including:
    + Domain: Defines the communication scope.
    + Type: Specifies the communication semantics.
    + Protocol: Determines how data is packaged and transmitted.

<p align="center">
  <img src="https://github.com/user-attachments/assets/f59fa01f-9b0b-4fed-a7a7-90e070290265" width="300">
</p>

### 6.1.2. Socket Domains
- The socket domain determines whether communication occurs between processes on the same machine or across different machines:
#### 6.1.2.1. UNIX Domain (AF_UNIX)
- Used for communication between processes on the same host.
- Highly efficient as it avoids network protocol overhead.
- Represented by a special file in the filesystem

#### 6.1.2.2. Internet Domain (AF_INET/AF_INET6)
- Used for communication between processes on different hosts.
- Supports IPv4 (AF_INET) and IPv6 (AF_INET6) addressing.
- Requires network protocol processing.

<p align="center">
  <img src="https://github.com/user-attachments/assets/5a0fe7e6-16d8-458a-97e8-109db05d0790" width="300">
</p>


### 6.1.3. Socket Types
- The socket type defines the communication semantics and data transfer mechanisms:
- Comparison of Socket Types

| Stream Socket (SOCK_STREAM) | Datagram Socket (SOCK_DGRAM) |
|-----------------------------|------------------------------|
| Reliable, connection-oriented communication| Unreliable, connectionless communication | 
| Guarantees ordered data delivery| No guarantee of data delivery order |
| Provides error notification | No error notification if data is lost |
| Requires connection establishment before data exchange| No connection needed; data can be sent immediately |
| Typically used with TCP protocol | Typically used with UDP protocol |
| Best for continuous data streams | Best for discrete packets of data |


### 6.1.4. Socket Protocols
- The protocol parameter specifies how data is packaged for transmission. While the domain and type often imply a specific protocol, this parameter allows for flexibility when multiple protocols support the same domain and type combination.
- In most cases, specifying 0 for the protocol parameter will select the default protocol for the chosen domain and type:
    + For AF_INET/SOCK_STREAM, the default is TCP.
    + For AF_INET/SOCK_DGRAM, the default is UDP

<p align="center">
  <img src="https://github.com/user-attachments/assets/32188714-5284-4480-b4ee-e798790a325d" width="300">
</p>

---

## 6.2. Socket Communication Flow
### 6.2.1. Stream Socket Flow (Connection-Oriented)
- Stream sockets require establishing a connection before data transmission, following a client-server model:
    1. Server creates a socket and binds it to an address.
    2. Server listens for incoming connections.
    3. Client creates a socket and connects to the server's address.
    4. Server accepts the connection, creating a new socket for this specific client.
    5. Data exchange occurs through send/recv calls.
    6. Either side can close the connection when finished

<p align="center">
  <img src="https://github.com/user-attachments/assets/628f38a7-fb48-484b-bd4d-4975ed4c13aa" width="50%">
</p>

### 6.2.2. Datagram Socket Flow (Connectionless)
- With datagram sockets, the distinction between client and server is less rigid:
    1. Both processes create sockets and bind them to addresses.
    2. Either process can send data to the other's address without prior connection.
    3. Data is received through recvfrom calls that also identify the sender.
    4. No formal connection termination is needed.

<p align="center">
  <img src="https://github.com/user-attachments/assets/8f85507b-45e9-4395-9cc3-295f5b92c1d1" width="50%">
</p>

---

## 6.3. Internet Domain Sockets
- Internet domain sockets enable communication between processes running on different machines across a network. They form the foundation of network programming in Linux and are essential for developing networked applications.

### 6.3.1. Internet Socket Addressing
- Internet domain sockets use the following address families:
    + **AF_INET:** Used for IPv4 communication.
    + **AF_INET6:** Used for IPv6 communication.
- The socket API in Linux provides a generic address structure (struct sockaddr) that serves as a common interface for all socket types:
```c
struct sockaddr {
    sa_family_t sa_family;     /* address family, AF_xxx */
    char        sa_data[14];   /* 14 bytes of protocol address */
};
```

#### 6.3.1.1. IPv4 Socket Address
- For IPv4 sockets, the address is represented by struct sockaddr_in, defined in `<netinet/in.h>:`

```c
struct sockaddr_in {         
    sa_family_t sin_family;    /* Address family (AF_INET) */ 
    in_port_t   sin_port;      /* Port number */ 
    struct in_addr sin_addr;   /* IPv4 address */ 
    unsigned char __pad[X];    /* Padding to match sockaddr size (16 bytes) */
};

struct in_addr {               /* IPv4 4-byte address */ 
    in_addr_t s_addr;          /* 32-bit IPv4 address in network byte order */
};

```
- Each IPv4 socket address consists of:
    + **sin_family:** Always set to AF_INET for IPv4 sockets.
    + **sin_port:** 16-bit port number in network byte order.
    + **sin_addr:** 32-bit IPv4 address in network byte order.
    + **__pad:** Padding bytes to ensure the structure size matches struct sockaddr.

- When working with IPv4 addresses, several special address values are commonly used:
    + **INADDR_ANY (0.0.0.0):** Binds a socket to all available network interfaces.
    + **INADDR_LOOPBACK (127.0.0.1):** Refers to the local loopback.
    + **INADDR_BROADCAST (255.255.255.255):** Used for broadcast messages.


#### 6.3.1.2. IPv6 Socket Address
- For IPv6 sockets, the address is represented by struct sockaddr_in6, also defined in `<netinet/in.h>:`

```c
struct sockaddr_in6 { 
    sa_family_t sin6_family;   /* Address family (AF_INET6) */ 
    in_port_t   sin6_port;     /* Port number */ 
    uint32_t    sin6_flowinfo; /* IPv6 flow information */ 
    struct in6_addr sin6_addr; /* IPv6 address */ 
    uint32_t    sin6_scope_id; /* Scope ID (interface index for link-local addresses) */
};

struct in6_addr {              /* IPv6 address structure */ 
    uint8_t s6_addr[16];       /* 16 bytes == 128 bits */
};

```
- Each IPv6 socket address consists of:
    + **sin6_family:** Always set to AF_INET6 for IPv6 sockets.
    + **sin6_port:** 16-bit port number in network byte order.
    + **sin6_flowinfo:** Traffic class and flow information.
    + **sin6_addr:** 128-bit IPv6 address.
    + **sin6_scope_id:** Identifies the network interface for scope-limited addresses.
- Special IPv6 addresses include:
    + `in6addr_any (::):` Binds to all interfaces.
    + `in6addr_loopback (::1):` Refers to the local loopback interface

### 6.3.2. Network Byte Order and Endianness
- One of the critical aspects of network programming is handling differences in how various computer architectures store multi-byte values in memory. This concept is known as "endianness":
  + **Little-endian:** Stores the least significant byte at the lowest memory address (used by x86 and x86-64 processors).
  + **Big-endian:** Stores the most significant byte at the lowest memory address (used by some RISC architectures).

<p align="center">
  <img src="https://github.com/user-attachments/assets/e6f64732-95ca-4b45-bdfe-5a5df2eedc3b" width="50%">
</p>

- To ensure consistent communication across different architectures, network protocols standardize on a single byte order called "network byte order," which is big-endian. This means:
    1. Before sending values over the network, the sender must convert from its host byte order to network byte order.
    2. After receiving values from the network, the receiver must convert from network byte order to its host byte order.

- For example, consider the 16-bit port number 8080 (0x1F90 in hexadecimal):
    + In little-endian: stored as 0x90 0x1F in memory.
    + In big-endian (network byte order): stored as 0x1F 0x90 in memory.
- If a little-endian machine sends the port number without conversion, a big-endian receiver would interpret it incorrectly as 36752 (0x8F90) instead of 8080.
    
### 6.3.3. Byte Order Conversion Functions
- To handle these byte order conversions, Linux provides a set of functions in `<arpa/inet.h>:`

```c
#include <arpa/inet.h>
#include <arpa/inet.h>

/* Host to Network conversions */
uint16_t htons(uint16_t host_uint16);   /* Host to Network Short (16-bit) */
uint32_t htonl(uint32_t host_uint32);   /* Host to Network Long (32-bit) */

/* Network to Host conversions */
uint16_t ntohs(uint16_t net_uint16);    /* Network to Host Short (16-bit) */
uint32_t ntohl(uint32_t net_uint32);    /* Network to Host Long (32-bit) */
```
- These functions perform the necessary byte swapping on little-endian machines, while on big-endian machines they are essentially no-ops (since host byte order already matches network byte order).
- **When to Use These Functions:**
    + **htons():** Convert port numbers before assigning to sin_port or sin6_port.
    + **htonl():** Convert IPv4 addresses before assigning to sin_addr.s_addr.
    + **ntohs():** Convert port numbers after retrieving from sin_port or sin6_port.
    + **ntohl():** Convert IPv4 addresses after retrieving from sin_addr.s_addr.
- For IPv6 addresses, which are 128 bits, you typically use the specialized functions `inet_pton()` and `inet_ntop()` instead, which handle the conversion implicitly.

#### Example: Using Network Byte Order Functions
```c
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    // Original values in host byte order
    uint16_t host_port = 8080;
    uint32_t host_addr = 0xC0A80101;  // 192.168.1.1 in hex
    
    // Convert to network byte order
    uint16_t net_port = htons(host_port);
    uint32_t net_addr = htonl(host_addr);
    
    printf("Original port: %u (0x%04X)\n", host_port, host_port);
    printf("Network byte order port: %u (0x%04X)\n", net_port, net_port);
    
    printf("Original address: 0x%08X\n", host_addr);
    printf("Network byte order address: 0x%08X\n", net_addr);
    
    // Create a socket address structure
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);  // Always convert to network byte order
    server_addr.sin_addr.s_addr = htonl(0xC0A80101);  // Always convert to network byte order
    
    // Retrieve and convert back to host byte order
    printf("Retrieved port: %u\n", ntohs(server_addr.sin_port));
    printf("Retrieved address: 0x%08X\n", ntohl(server_addr.sin_addr.s_addr));
    
    return 0;
}
```
- On a little-endian machine (like x86), this program would output:
```bash
Original port: 8080 (0x1F90)
Network byte order port: 36895 (0x901F)  // Bytes swapped for display
Original address: 0xC0A80101
Network byte order address: 0x0101A8C0  // Bytes swapped for display
Retrieved port: 8080
Retrieved address: 0xC0A80101
```
- Note that the actual values in memory are properly converted, but when printed as integers, they appear different because the printf function interprets them according to the host byte order.

#### Additional Address Conversion Functions
- For working with human-readable IP address strings, Linux provides these functions:
```c
#include <arpa/inet.h>

/* Convert string to binary address */
int inet_pton(int af, const char *src, void *dst);

/* Convert binary address to string */
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```
- Example usage:
```c
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
inet_pton(AF_INET, "192.168.1.1", &server_addr.sin_addr);

// Later, to display the address:
char ip_str[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &server_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
printf("Server IP: %s\n", ip_str);
```

### 6.3.4. Complete TCP/IP Example
#### TCP Server Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept connections on any interface
    server_addr.sin_port = htons(PORT);
    
    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d\n", PORT);
    
    // Accept and handle connections
    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Accept failed");
            continue;
        }
        
        // Convert client IP to string for display
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        
        // Read data from client
        ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Received: %s\n", buffer);
            
            // Echo back to client
            send(client_fd, buffer, bytes_read, 0);
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    return 0;
}
```

#### TCP Client Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }
    
    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server. Enter message: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    
    // Remove newline character
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n')
        buffer[--len] = '\0';
    
    // Send message to server
    send(sock_fd, buffer, len, 0);
    printf("Message sent\n");
    
    // Receive response
    ssize_t bytes_read = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
    buffer[bytes_read] = '\0';
    printf("Server response: %s\n", buffer);
    
    close(sock_fd);
    return 0;
}
```
#### UDP Server Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Create UDP socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    
    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    printf("UDP server listening on port %d\n", PORT);
    
    // Receive and respond to datagrams
    while (1) {
        ssize_t bytes_read = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                     (struct sockaddr *)&client_addr, &client_len);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            
            // Convert client IP to string for display
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            printf("Received from %s:%d: %s\n", 
                   client_ip, ntohs(client_addr.sin_port), buffer);
            
            // Echo back to client
            sendto(server_fd, buffer, bytes_read, 0,
                  (struct sockaddr *)&client_addr, client_len);
        }
    }
    
    close(server_fd);
    return 0;
}
```

#### UDP Client Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE];
    
    // Create UDP socket
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }
    
    printf("Enter message: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    
    // Remove newline character
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n')
        buffer[--len] = '\0';
    
    // Send message to server
    sendto(sock_fd, buffer, len, 0, 
          (struct sockaddr *)&server_addr, server_len);
    printf("Message sent\n");
    
    // Receive response
    ssize_t bytes_read = recvfrom(sock_fd, buffer, BUFFER_SIZE - 1, 0,
                                 (struct sockaddr *)&server_addr, &server_len);
    buffer[bytes_read] = '\0';
    printf("Server response: %s\n", buffer);
    
    close(sock_fd);
    return 0;
}
```

---

## 6.4. Unix Domain Sockets
- Unix Domain Sockets provide high-performance inter-process communication (IPC) between processes on the same host. They offer significant advantages over Internet sockets for local communication.
- **Domain:** `AF_UNIX` (or `AF_LOCAL`)
- `Socket Types:`
    + `SOCK_STREAM:` Connection-oriented, reliable communication (like TCP).
    + `SOCK_DGRAM:` Connectionless communication with message boundaries (like UDP).
- **Protocol:** Always 0.
- **Performance:** Faster than Internet sockets for local communication (no protocol overhead).
- **Key Advantages:**
    + Avoid network stack processing.
    + Higher throughput and lower latency.
    + Can pass file descriptors between processes.
    + Security through filesystem permissions.
    
### 6.4.1. Socket Addressing
- Unix Domain Sockets use filesystem pathnames for addressing:
```c
struct sockaddr_un {
    sa_family_t sun_family;    /* Always AF_UNIX */
    char sun_path[108];        /* Null-terminated socket pathname */
};
```

### 6.4.2. Socket File Managements
-  When binding a Unix Domain Socket:
    + A special socket file is created at the specified pathname.
    + The file appears in the filesystem but **cannot be accessed with regular file operations**.
    + The pathname must not already exist when binding.
    + The socket file persists even after the socket is closed or the program terminates.
    + Manual cleanup with `unlink()` is required.
    
```c
// Create and bind socket
int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
struct sockaddr_un addr;
addr.sun_family = AF_UNIX;
strcpy(addr.sun_path, "/tmp/my_socket");

// Remove any existing socket file first
unlink("/tmp/my_socket");

// Bind socket to address
bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

// When done, clean up
close(sockfd);
unlink("/tmp/my_socket");

```

### 6.4.4. Access Control
- Unix Domain Sockets use filesystem permissions for access control:
    + **Connection/Send:** Requires write permission on the socket file.
    + **Receive:** Requires read permission on the socket file.
- Permission control methods:
```c
// Using umask before binding
mode_t old_mask = umask(077);  // Only allow owner access
bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
umask(old_mask);

// Using chmod after binding
bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
chmod("/tmp/my_socket", S_IRUSR | S_IWUSR);  // Read/write for owner only

```

### 6.4.5. Server/Client Model
#### Stream Socket Server Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/example_socket"
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Remove any existing socket file
    unlink(SOCKET_PATH);
    
    // Bind socket to address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    // Set socket permissions
    chmod(SOCKET_PATH, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);  // 0660 permissions
    
    // Listen for connections
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on %s\n", SOCKET_PATH);
    
    // Accept loop
    while (1) {
        client_len = sizeof(client_addr);
        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) == -1) {
            perror("accept");
            continue;
        }
        
        printf("Client connected\n");
        
        // Get client credentials
        struct ucred cred;
        socklen_t len = sizeof(cred);
        if (getsockopt(client_fd, SOL_SOCKET, SO_PEERCRED, &cred, &len) == -1) {
            perror("getsockopt");
        } else {
            printf("Client PID: %d, UID: %d, GID: %d\n", cred.pid, cred.uid, cred.gid);
        }
        
        // Echo back received data
        ssize_t bytes_read;
        while ((bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[bytes_read] = '\0';
            printf("Received: %s\n", buffer);
            send(client_fd, buffer, bytes_read, 0);
        }
        
        if (bytes_read == -1) {
            perror("recv");
        }
        
        close(client_fd);
        printf("Client disconnected\n");
    }
    
    // Cleanup
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
```

#### Stream Socket Client Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/example_socket"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server. Type messages (Ctrl+D to quit):\n");
    
    // Send/receive loop
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            len--;
        }
        
        if (send(client_fd, buffer, len, 0) == -1) {
            perror("send");
            break;
        }
        
        ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read <= 0) {
            if (bytes_read < 0)
                perror("recv");
            else
                printf("Server closed connection\n");
            break;
        }
        
        buffer[bytes_read] = '\0';
        printf("Echo: %s\n", buffer);
    }
    
    // Cleanup
    close(client_fd);
    return 0;
}
```

#### Datagram Socket Server Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "/tmp/dgram_server_socket"
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    // Create a datagram (UDP-like) socket
    if ((server_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_PATH, sizeof(server_addr.sun_path) - 1);

    // Remove any existing socket file
    unlink(SERVER_PATH);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s\n", SERVER_PATH);

    // Receive and respond to data loop
    while (1) {
        client_len = sizeof(client_addr);
        ssize_t bytes_read = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr*)&client_addr, &client_len);

        if (bytes_read == -1) {
            perror("recvfrom");
            continue;
        }

        buffer[bytes_read] = '\0';
        printf("Received: %s\n", buffer);

        // Send the data back to the client (echo)
        if (sendto(server_fd, buffer, bytes_read, 0,
                   (struct sockaddr*)&client_addr, client_len) == -1) {
            perror("sendto");
        }
    }

    // Cleanup resources
    close(server_fd);
    unlink(SERVER_PATH);
    return 0;
}
```

#### Datagram Socket Server Example
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "/tmp/dgram_server_socket"
#define CLIENT_PATH "/tmp/dgram_client_socket"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // Create a datagram socket
    if ((client_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configure the client address (to receive responses)
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, CLIENT_PATH, sizeof(client_addr.sun_path) - 1);

    // Remove any existing socket file
    unlink(CLIENT_PATH);

    // Bind the socket to the client address
    if (bind(client_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
        perror("bind");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_PATH, sizeof(server_addr.sun_path) - 1);

    // Send data to the server
    const char *message = "Hello from client!";
    if (sendto(client_fd, message, strlen(message), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("sendto");
        close(client_fd);
        unlink(CLIENT_PATH);
        exit(EXIT_FAILURE);
    }

    // Receive response from the server
    socklen_t server_len = sizeof(server_addr);
    ssize_t bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr*)&server_addr, &server_len);
    if (bytes_received == -1) {
        perror("recvfrom");
    } else {
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    // Cleanup resources
    close(client_fd);
    unlink(CLIENT_PATH);
    return 0;
}
```