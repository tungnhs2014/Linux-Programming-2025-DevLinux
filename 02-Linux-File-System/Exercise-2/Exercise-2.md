## Exercise 2

After each `write` command below, determine which file the content will be written to, what the content will be, and explain why.

```c
fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
fd3 = open(file, O_RDWR);
write(fd1, "Hello,", 6);
write(fd2, "world", 6);
lseek(fd2, 0, SEEK_SET);
write(fd1, "HELLO,", 6);
write(fd3, "Gidday", 6);
```

## Answer
### Open Data
```c
fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
```
- Opens or creates a file with read (`S_IRUSR`) and write (`S_IWUSR`) permissions.
- `O_TRUNC` clears the content if the file already exists.

```c
fd3 = open(file, O_RDWR);
```
- Opens the same file with read/write (`O_RDWR`) permissions.
- The file remains unchanged.

### Writing Data

```c
write(fd1, "Hello,", 6);
```
- Writes `"Hello,"` (6 bytes) at the beginning (`offset = 0`).
- **File content after this step:**
  ```
  Hello,
  ```

```c
write(fd2, "world", 6);
```
- **Error!** `fd2` was not opened using `open()`, so `write()` **fails**.
- **File remains unchanged:**
  ```
  Hello,
  ```

```c
lseek(fd2, 0, SEEK_SET);
```
- **Error!** `fd2` is invalid, so `lseek()` fails.
- No changes to the file.

```c
write(fd1, "HELLO,", 6);
```
- `fd1` is at `offset = 6` (after `"Hello,"`).
- Writes `"HELLO,"` at this position.
- **File content after this step:**
  ```
  Hello,HELLO,
  ```

```c
write(fd3, "Gidday", 6);
```
- `fd3` writes `"Gidday"` from the beginning (`offset = 0`).
- Overwrites `"Hello,"`.
- **Final file content:**
  ```
  GiddayHELLO,
  ```

