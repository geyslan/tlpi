### 6-1

> Compile the program in Listing 6-1 ( mem_segments.c ), and list its size using ls –l. Although the program contains an array (mbuf) that is around 10 MB in size, the executable file is much smaller than this. Why is this?

- It's because `mbuf` symbol is allocated in the uninitialized data segment (bss - block started by symbol). But don't be confused, even if `mbuf` is in the uninitialized data segment, the content that this symbol references is zeroed, as it's `static` and not `auto`matic.

### 6-2

> Write a program to see what happens if we try to longjmp() into a function that has already returned.

- `nolongjmp.c` is the testcase. It crashes because it was created with that goal. But the truth is that returning from a function that calls `setjmp()` leads to undefined behavior, once `longjmp()` call isn't nested in the inner stack frames.

### 6-3

> Implement setenv() and unsetenv() using getenv(), putenv(), and, where necessary, code that directly modifies environ. Your version of unsetenv() should check to see whether there are multiple definitions of an nvironment variable, and remove them all (which is what the glibc version of unsetenv() does).

- `./env "SHELL" "/bin/sh" x`