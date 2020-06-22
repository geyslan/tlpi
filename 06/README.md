### 6-1

> Compile the program in Listing 6-1 ( mem_segments.c ), and list its size using ls –l. Although the program contains an array (mbuf) that is around 10 MB in size, the executable file is much smaller than this. Why is this?

- It's because `mbuf` symbol is allocated in the uninitialized data segment (bss - block started by symbol). But don't be confused, even if `mbuf` is in the uninitialized data segment, the content that this symbol references is zeroed, as it's `static` and not `auto`matic.