#include "common.h"
#include "error.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct my_iovec {
	void * iov_base;
	size_t iov_len;
};

ssize_t my_readv(int fildes, const struct my_iovec *iov, int iovcnt)
{
	long	openmax;
	char *	buffer;
	ssize_t bufsize = 0;
	ssize_t nread;
	ssize_t ncopied = 0;

	openmax = sysconf(_SC_IOV_MAX);
	if (iovcnt <= 0 || iovcnt > _SC_IOV_MAX) {
		errno = EINVAL;
		return -1;
	}
	if (!iov)
		return -1;
	for (int i = 0; i < iovcnt; ++i) {
		if (!iov[i].iov_base)
			return -1;

		if (bufsize > SIZE_MAX - iov[i].iov_len) {
			errno = EINVAL;
			return -1;
		}
		bufsize += iov[i].iov_len;
	}

	buffer = malloc(bufsize);
	if (!buffer)
		return -1;
	nread = read(fildes, buffer, bufsize);
	if (nread <= 0)
		goto bail;
	for (int i = 0; i < iovcnt && ncopied < nread; ++i) {
		void *	base = iov[i].iov_base;
		ssize_t len  = iov[i].iov_len;

		len = min(len, nread - ncopied);
		memcpy(base, &buffer[ncopied], len);
		ncopied += len;
	}
bail:
	free(buffer);
	return nread;
}

#define STR_SIZE 10000

int main(int argc, char *argv[])
{
	int		fd;
	struct my_iovec iov[3];
	struct stat	my_struct; /* First buffer */
	int		x; /* Second buffer */
	char		str[STR_SIZE]; /* Third buffer */
	ssize_t		num_read, tot_required;

	app_set_name(argv[0]);

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		errx("%s file\n", app_get_name());
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		errx("open");

	tot_required	= 0;
	iov[0].iov_base = &my_struct;
	iov[0].iov_len	= sizeof(struct stat);
	tot_required += iov[0].iov_len;

	iov[1].iov_base = &x;
	iov[1].iov_len	= sizeof(x);
	tot_required += iov[1].iov_len;

	iov[2].iov_base = str;
	iov[2].iov_len	= STR_SIZE;
	tot_required += iov[2].iov_len;

	num_read = my_readv(fd, iov, 3);
	if (num_read == -1)
		errx("readv");
	if (num_read < tot_required)
		printf("Read fewer bytes than requested\n");
	printf("total bytes requested: %ld; bytes read: %ld\n",
	       (long)tot_required, (long)num_read);

	return 0;
}