#include "common.h"
#include "error.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int my_dup(int fildes)
{
	return fcntl(fildes, F_DUPFD, 0);
}

int my_dup2(int fildes, int fildes2)
{
	int  newfd;
	int  flags;
	long openmax;

	openmax = sysconf(_SC_OPEN_MAX);

	if (fildes < 0 || fildes2 < 0 || fildes2 >= openmax) {
		errno = EBADF;
		return -1;
	}

	if (fcntl(fildes, F_GETFD) == -1)
		return -1;
	if (fildes == fildes2)
		return fildes2;
	if (fcntl(fildes2, F_GETFD) != -1)
		close(fildes2);
	if (newfd = fcntl(fildes, F_DUPFD, fildes2) == -1)
		return -1;
	if (flags = fcntl(newfd, F_GETFD) == -1)
		return -1;
	flags &= ~FD_CLOEXEC;
	if (fcntl(newfd, F_SETFD, flags) == -1)
		return -1;

	return newfd;
}

int main(int argc, char *argv[])
{
	int fd;

	fd = my_dup(2);
	my_dup2(fd, 34);

	sleep(10);

	return 0;
}