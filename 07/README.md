### 7-1

> Modify the program in Listing 7-1 (free_and_sbrk.c) to print out the current value of the program break after each execution of malloc(). Run the program specifying a small allocation block size. This will demonstrate that malloc() doesn't employ sbrk() to adjust the program break on each call, but instead periodically allocates larger chunks of memory from which it passes back small pieces to the caller.

`./free_and_sbrk 15 10000 2`
```
Initial program break: 0x559c7b531000
Allocating 15*10000 bytes
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b531000
Program break is now: 0x559c7b553000
Program break is now: 0x559c7b553000
Program break is now: 0x559c7b553000
Freeing blocks from 1 to 15 in steps of 2
After free(), program break is: 0x559c7b553000
```

`./free_and_sbrk 50 5000 2`
```
Initial program break: 0x563d33a59000
Allocating 50*5000 bytes
Program break is now: 0x563d33a59000
Program break is now: 0x563d33a59000
Program break is now: 0x563d33a59000
Program break is now: 0x563d33a59000
...
Program break is now: 0x563d33a59000
Program break is now: 0x563d33a59000
Program break is now: 0x563d33a59000
Program break is now: 0x563d33a59000
Program break is now: 0x563d33a7a000
Program break is now: 0x563d33a7a000
Program break is now: 0x563d33a7a000
Program break is now: 0x563d33a7a000
...
Program break is now: 0x563d33a7a000
Program break is now: 0x563d33a7a000
Program break is now: 0x563d33a7a000
Program break is now: 0x563d33a7a000
Freeing blocks from 1 to 50 in steps of 2
After free(), program break is: 0x563d33a7a000
```

### 7-2

> (Advanced) Implement malloc() and free().

- Well, `my_malloc()` and `my_free()` work. But they don't care about merging free chunks or sorting them. They just work, surely with bugs and with a lot of fragmentation. =D

```
$ ./malloc 
----------------------------------------
program break:  0x55c7ee4ce000

----------------------------------------
program break:  0x55c7ee4cf000
(1) FREE CHUNK
 chunk address: 0x55c7ee4ce022
 chunk size:    4062
  chunk prev:   (nil)
  chunk next:   (nil)


----------------------------------------
program break:  0x55c7ee4cf000
(1) FREE CHUNK
 chunk address: 0x55c7ee4ce044
 chunk size:    4028
  chunk prev:   (nil)
  chunk next:   (nil)


----------------------------------------
program break:  0x55c7ee4cf000
(1) FREE CHUNK
 chunk address: 0x55c7ee4ce07f
 chunk size:    3969
  chunk prev:   (nil)
  chunk next:   0x55c7ee4ce022

(2) FREE CHUNK
 chunk address: 0x55c7ee4ce022
 chunk size:    10
  chunk prev:   0x55c7ee4ce07f
  chunk next:   (nil)


----------------------------------------
program break:  0x55c7ee4d1728
(1) FREE CHUNK
 chunk address: 0x55c7ee4ce000
 chunk size:    10
  chunk prev:   (nil)
  chunk next:   0x55c7ee4ce07f

(2) FREE CHUNK
 chunk address: 0x55c7ee4ce07f
 chunk size:    3969
  chunk prev:   0x55c7ee4ce000
  chunk next:   0x55c7ee4ce022

(3) FREE CHUNK
 chunk address: 0x55c7ee4ce022
 chunk size:    10
  chunk prev:   0x55c7ee4ce07f
  chunk next:   (nil)


----------------------------------------
program break:  0x55c7ee4d1728
(1) FREE CHUNK
 chunk address: 0x55c7ee4ce066
 chunk size:    1
  chunk prev:   (nil)
  chunk next:   0x55c7ee4ce000

(2) FREE CHUNK
 chunk address: 0x55c7ee4ce000
 chunk size:    10
  chunk prev:   0x55c7ee4ce066
  chunk next:   0x55c7ee4ce07f

(3) FREE CHUNK
 chunk address: 0x55c7ee4ce07f
 chunk size:    3969
  chunk prev:   0x55c7ee4ce000
  chunk next:   0x55c7ee4ce022

(4) FREE CHUNK
 chunk address: 0x55c7ee4ce022
 chunk size:    10
  chunk prev:   0x55c7ee4ce07f
  chunk next:   (nil)


----------------------------------------
program break:  0x55c7ee4d1728
(1) FREE CHUNK
 chunk address: 0x55c7ee4cf20c
 chunk size:    9500
  chunk prev:   (nil)
  chunk next:   0x55c7ee4ce066

(2) FREE CHUNK
 chunk address: 0x55c7ee4ce066
 chunk size:    1
  chunk prev:   0x55c7ee4cf20c
  chunk next:   0x55c7ee4ce000

(3) FREE CHUNK
 chunk address: 0x55c7ee4ce000
 chunk size:    10
  chunk prev:   0x55c7ee4ce066
  chunk next:   0x55c7ee4ce07f

(4) FREE CHUNK
 chunk address: 0x55c7ee4ce07f
 chunk size:    3969
  chunk prev:   0x55c7ee4ce000
  chunk next:   0x55c7ee4ce022

(5) FREE CHUNK
 chunk address: 0x55c7ee4ce022
 chunk size:    10
  chunk prev:   0x55c7ee4ce07f
  chunk next:   (nil)


----------------------------------------
program break:  0x55c7ee4d1728
(1) FREE CHUNK
 chunk address: 0x55c7ee4ce044
 chunk size:    10
  chunk prev:   (nil)
  chunk next:   0x55c7ee4cf000

(2) FREE CHUNK
 chunk address: 0x55c7ee4cf000
 chunk size:    500
  chunk prev:   0x55c7ee4ce044
  chunk next:   0x55c7ee4cf20c

(3) FREE CHUNK
 chunk address: 0x55c7ee4cf20c
 chunk size:    9500
  chunk prev:   0x55c7ee4cf000
  chunk next:   0x55c7ee4ce066

(4) FREE CHUNK
 chunk address: 0x55c7ee4ce066
 chunk size:    1
  chunk prev:   0x55c7ee4cf20c
  chunk next:   0x55c7ee4ce000

(5) FREE CHUNK
 chunk address: 0x55c7ee4ce000
 chunk size:    10
  chunk prev:   0x55c7ee4ce066
  chunk next:   0x55c7ee4ce07f

(6) FREE CHUNK
 chunk address: 0x55c7ee4ce07f
 chunk size:    3969
  chunk prev:   0x55c7ee4ce000
  chunk next:   0x55c7ee4ce022

(7) FREE CHUNK
 chunk address: 0x55c7ee4ce022
 chunk size:    10
  chunk prev:   0x55c7ee4ce07f
  chunk next:   (nil)
```