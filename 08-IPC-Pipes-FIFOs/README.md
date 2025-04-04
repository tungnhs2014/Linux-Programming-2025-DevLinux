# IPC - Pipes and FIFOs

## 1. Introduction to Pipes
### 1.1. Definition
- Pipes are an Inter-Process Communication (IPC) mechanism used for communication between related processes in Linux.
- Pipes are unidirectional, meaning data flows in only one direction: one process writes to the pipe while another reads from it.
- They provide a simple yet effective method for parent-child process communication.

<p align="center">
  <img src="https://github.com/user-attachments/assets/9fa07748-8abc-467c-ac85-276cc8aa9446" alt="Image" width="400"/>
</p>

### 1.2. Pipe Operation
- When a pipe is created, it exists in RAM as a **"virtual file"** with a fixed buffer size (typically 65536 bytes in modern Linux systems).
- Pipes can be created before forking processes to establish communication channels between **parent and child processes**.
- Data written to a pipe by one process can be read by another related process.
- Real-world analogy: Like pouring water (data) into a physical pipe that flows into a container, where someone can collect it with a cup. The output of the pipe becomes the input for the recipient.

<p align="center">
  <img src="https://github.com/user-attachments/assets/5f3ae4ba-57a5-4714-b8a6-9ff6ef94b293" alt="Image" width="400"/>
</p>


#### 1.2.1. Reading from a pipe
- When attempting to read from an empty pipe, the reading process blocks until at least one byte becomes available.
- If all write ends of a pipe are closed, a read operation will return all remaining data and then return 0 (EOF).
- This ensures processes synchronize naturally when exchanging data.

#### 1.2.2. Pipes have a limited capacity
- Pipes have a limited capacity (buffer size).
- When a pipe is full, the writing process blocks until some data is read from the pipe.
- If all read ends of a pipe are closed, a write operation will cause the writing process to receive a SIGPIPE signal, which by default terminates the process.

--- 

## 2. Creating and Using Pipes
### 2.1. Creating Pipes
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

### 2.2. Bidirectional Communication
- A single pipe allows communication in only one direction.
- For bidirectional communication, two pipes are needed - one for each direction.
- When implementing bidirectional communication, care must be taken to avoid deadlock situations where both processes are blocked waiting for each other.

<p align="center">
  <img src="https://github.com/user-attachments/assets/e6991e17-1fcc-44bc-b451-35014ac77f9b" alt="Two-way Pipe Communication" width="400"/>
</p>


- Parent đóng vai trò writer.
- Child đóng vai trò reader.

<p align="center">
  <img src="https://github.com/user-attachments/assets/fa51ed86-96d3-49ca-abb2-f1c676725b36" alt="Parent as Writer - Child as Reader" width="400"/>
</p>


- Parent đóng vai trò reader.
- Child đóng vai trò writer.

![Image](https://github.com/user-attachments/assets/f0b83f9c-6666-4e8c-9d0e-b9407c6c3608)

--- 

## 3. FIFOs – named Pipes
### 3.1. Định nghĩa
- Đây là một khái niệm mở rộng của pipes. Pipes truyền thống thì không được đặt tên và chỉ tồn tại trong suốt vòng đời của process.
- Sự khác biệt chính là FIFOs có tên trong hệ thống tệp và được mở giống như một tệp thông thường.
- Named Pipes có thể tồn tại miễn là hệ thống còn hoạt động. Vượt ra ngoài vòng đời của process. Có thể xóa đi nếu không còn sử dụng.
- Một file FIFO là một file đặc biệt được lưu trong bộ nhớ cục bộ. được tạo ra bởi hàm mkfifo() trong C. 

### 3.2. Tạo FIFOs từ trình shell
-  Lệnh: mkfifo [ -m mode ] pathname
    + Ex: mkfifo -m 0666 ./helloFIFO

![Image](https://github.com/user-attachments/assets/ec3a4ff3-ec47-48e1-8b30-52d6fd4eb481)

### 3.3. Tạo FIFOs từ source code
- mkfifo().
- FIFOs là một loại tệp, chúng ta có thể sử dụng tất cả các lệnh gọi hệ thống được liên kết với nó i.e. open, read, write, close.
```c
int mkfifo(const char *pathname, mode_t mode);
```

- Tạo một FIFO mới với đường dẫn cụ thể. Trả về hai file descriptor (fd) nằm trong fds.
- Các đối số:
    + pathname:	tên file FIFO.	
    + mode: các quyền đối với file FIFO.
    + Trả về 0 nếu thành công, -1 nếu thất bại.


## 4. Xây dựng mô hình Client-Server với FIFOs
Bài toán: Xây dựng một mô hình client server sử dụng FIFOs.
### 4.1 B1: Thiết lập kết nối
- Làm sao để clients biết tới server?
- well-know address/name.
   + Vị trí cố định.
- Server được định danh.
  + Cung cấp nhiều services.
  + Client phải request tới để đăng kí.
  + Thêm chi phí (tiền, lập trình).

### 4.2 Xử lý yêu cầu kết nối. 
- Nhiều clients request, không thể sử dụng 1 FIFO để xử lý toàn bộ

#### Server
- Tạo một FIFO xử lý request.
- Xử lý bản tin request.
  + Check key.
  + Check service.
  + Giới hạn số lượng kết nối.
- Gửi ack chấp nhận/không chấp nhận request.

#### Client
- Tạo FIFO gửi nhận bản tin
  + Tên = TEMPLATE+PID
- Đóng bản tin request 
  + Tên
  + Yêu cầu service nào.
  + Key
- Gửi request tới server
  + Server chấp nhận ---> Gửi nhận dữ liệu
  + Server ko chấp nhận ---> Hủy bỏ FIFO

### 4.3: Xử lý bản tin.
- Nhớ lại rằng dữ liệu trong đường ống và FIFO là một luồng byte, ranh giới giữa nhiều bản tin không được bảo toàn.
- Điều này có nghĩa là khi nhiều bản tin gửi đến một quá trình, chẳng hạn như server, thì người gửi và người nhận phải đồng ý về một số quy ước để tách các thư. 
  + Kết thúc mỗi thư bằng ký tự phân cách.
    + Ký tự không bao giờ xuất hiện như một phần của bản tin.
    + Quá trình đọc thông báo phải quét dữ liệu từ FIFO từng byte một cho đến khi tìm thấy ký tự phân tách.
- Bao gồm header có kích thước cố định với trường độ dài trong mỗi bản tin.
    + Hiệu quả  đối với các bản tin có kích thước tùy ý.
    + Dẫn đến sự cố nếu bản tin không đúng định dạng.
- Sử dụng các bản tin có độ dài cố định và yêu cầu server luôn đọc các bản tin có kích thước cố định này.
    + Điều này có lợi thế là đơn giản để lập trình.
    + Dung lượng kênh truyền bị lãng phí.
    + Nếu một tin nhắn không có độ dài phù hợp, thì tất cả các tin nhắn tiếp theo sẽ bị lệch. Trong trường hợp này, server không thể khôi phục dễ dàng.