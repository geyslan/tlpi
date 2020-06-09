#include "common.h"
#include "error.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void)
{
	int   oldfd;
	int   dupfd;
	int   oldflags;
	int   dupflags;
	off_t oldoff = 0, dupoff = 0;

	oldfd = open("testfile", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

	dupfd  = dup2(oldfd, 33);
	oldoff = lseek(oldfd, 0, SEEK_CUR);
	dupoff = lseek(dupfd, 0, SEEK_CUR);

	printf("[offset] old: %d - dup: %d\n", oldoff, dupoff);

	write(oldfd, "ABCD", 4);

	oldoff = lseek(oldfd, 0, SEEK_CUR);
	dupoff = lseek(dupfd, 0, SEEK_CUR);

	printf("[offset] old: %d - dup: %d\n", oldoff, dupoff);
	printf("[offset] %s\n", (oldoff == dupoff) ? "same" : "not same");

	oldflags = fcntl(oldfd, F_GETFL);
	dupflags = fcntl(dupfd, F_GETFL);

	printf("[flags] old: 0x%x - dup: 0x%x\n", oldflags, dupflags);
	printf("[flags] %s\n", (oldflags == dupflags) ? "same" : "not same");

	close(oldfd);
	close(dupfd);
	unlink("testfile");

	return 0;
}