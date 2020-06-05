### 5-1

> Modify the program in Listing 5-3 to use standard file I/O system calls (open() and lseek()) and the off_t data type. Compile the program with the _FILE_OFFSET_BITS macro set to 64, and test it to show that a large file can be successfully created.

- `largefile` creates large files successfully.

### 5-2

> Write a program that opens an existing file for writing with the O_APPEND flag, and then seeks to the beginning of the file before writing some data. Where does the data appear in the file? Why?

- `oappend` always write to the end of the open file because the `O_APPEND` flag makes the file offset to be set to the end of the file prior to each write.