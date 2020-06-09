### 5-1

> Modify the program in Listing 5-3 to use standard file I/O system calls (open() and lseek()) and the off_t data type. Compile the program with the _FILE_OFFSET_BITS macro set to 64, and test it to show that a large file can be successfully created.

- `largefile` creates large files successfully.

### 5-2

> Write a program that opens an existing file for writing with the O_APPEND flag, and then seeks to the beginning of the file before writing some data. Where does the data appear in the file? Why?

- `oappend` writes to the end of the open file in despite of the previous set to 0 offset because the `O_APPEND` flag makes the file offset to be set to the end of the file prior to each write.

### 5-3

> This exercise is designed to demonstrate why the atomicity guaranteed by opening a file with the O_APPEND flag is necessary. Write a program that takes up to three command-line arguments:

`$ atomic_append filename num-bytes [x]`

> This file should open the specified filename (creating it if necessary) and append num-bytes bytes to the file by using write() to write a byte at a time. By default, the program should open the file with the O_APPEND flag, but if a third command-line argument (x) is supplied, then the O_APPEND flag should be omitted, and instead the program should perform an lseek(fd, 0, SEEK_END) call before each write(). Run two instances of this program at the same time without the x argument to write 1 million bytes to the same file:

`$ atomic_append f1 1000000 & atomic_append f1 1000000`

> Repeat the same steps, writing to a different file, but this time specifying the x argument:

`$ atomic_append f2 1000000 x & atomic_append f2 1000000 x`

> List the sizes of the files f1 and f2 using ls -l and explain the difference.

```
$ ls -l f1 f2
-rw-r--r-- 1 uzu users 4000000 jun  5 11:27 f1
-rw-r--r-- 1 uzu users 2000000 jun  5 11:28 f2
```

- Using the option `x` creates a race condition as the write calls are not atomic due the lack of O_APPEND. The time slice of the first process elapses before the writing comes to its end making the second process not to write in the rightful offset.

### 5-4

> Implement dup() and dup2() using fcntl() and, where necessary, close(). (You may ignore the fact that dup2() and fcntl() return different errno values for some error cases.) For dup2(), remember to handle the special case where oldfd equals newfd. In this case, you should check whether oldfd is valid, which can be done by, for example, checking if fcntl(oldfd, F_GETFL) succeeds. If oldfd is not valid, then the function should return –1 with errno set to EBADF.

```
$ ./dup 2>/dev/null & ls -l /proc/$(echo -n `pgrep dup`)/fd/
[1] 364105
total 0
lrwx------ 1 uzu users 64 jun  8 13:05 0 -> /dev/pts/0
lrwx------ 1 uzu users 64 jun  8 13:05 1 -> /dev/pts/0
l-wx------ 1 uzu users 64 jun  8 13:05 2 -> /dev/null
l-wx------ 1 uzu users 64 jun  8 13:05 3 -> /dev/null
l-wx------ 1 uzu users 64 jun  8 13:05 34 -> /dev/null
```

### 5-5

> Write a program to verify that duplicated file descriptors share a file offset value and open file status flags.

```
$ ./dup_check 
[offset] old: 0 - dup: 0
[offset] old: 4 - dup: 4
[offset] same
[flags] old: 0x2 - dup: 0x2
[flags] same
```

<cite> Two different file descriptors that refer to the same open file description share a file offset value. Therefore, if the file offset is changed via one file descriptor (as a consequence of calls to read(), write(), or lseek()), this change is visible through the other file descriptor. This applies both when the two file descriptors belong to the same process and when they belong to different processes.</cite>