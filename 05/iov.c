#include "common.h"
#include "error.h"
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct my_iovec {
	void * iov_base;
	size_t iov_len;
};

ssize_t get_buf_size(const struct my_iovec *iov, int iovcnt)
{
	ssize_t bufsize = 0;

	for (int i = 0; i < iovcnt; ++i) {
		if (!iov[i].iov_base)
			return -1;

		if (bufsize > SSIZE_MAX - iov[i].iov_len) {
			errno = EINVAL;
			return -1;
		}
		bufsize += iov[i].iov_len;
	}

	return bufsize;
}

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
	bufsize = get_buf_size(iov, iovcnt);
	if (bufsize == -1)
		return -1;

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

ssize_t my_writev(int fildes, const struct my_iovec *iov, int iovcnt)
{
	long	openmax;
	char *	buffer;
	ssize_t bufsize = 0;
	ssize_t nwritten;
	ssize_t ncopied = 0;

	openmax = sysconf(_SC_IOV_MAX);
	if (iovcnt <= 0 || iovcnt > _SC_IOV_MAX) {
		errno = EINVAL;
		return -1;
	}
	if (!iov)
		return -1;
	bufsize = get_buf_size(iov, iovcnt);
	if (bufsize == -1)
		return -1;

	buffer = malloc(bufsize);
	if (!buffer)
		return -1;
	for (int i = 0; i < iovcnt; ++i) {
		void *	base = iov[i].iov_base;
		ssize_t len  = iov[i].iov_len;

		memcpy(&buffer[ncopied], base, len);
		ncopied += len;
	}
	nwritten = write(fildes, buffer, bufsize);

	free(buffer);
	return nwritten;
}

#define STR_SIZE 10000

int main(int argc, char *argv[])
{
	int		fd;
	struct my_iovec iov_w[3], iov_r[3];
	struct stat	my_struct_w = {}, my_struct_r = {}; /* First buffer */
	int		x_w, x_r; /* Second buffer */
	char	str_w[STR_SIZE] = {}, str_r[STR_SIZE] = {}; /* Third buffer */
	ssize_t num_written, num_read, tot_required;
	int	flag = O_CREAT | O_WRONLY | O_TRUNC;
	mode_t	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	app_set_name(argv[0]);

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		errx("%s file\n", app_get_name());

	my_struct_w.st_uid  = 1000;
	my_struct_w.st_gid  = 1000;
	my_struct_w.st_mode = mode;
	x_w		    = ~0;
	strcpy(str_w, "Testing my iov functions!!!");

	fd = open(argv[1], flag, mode);
	if (fd == -1)
		errx("open");

	tot_required	  = 0;
	iov_w[0].iov_base = &my_struct_w;
	iov_w[0].iov_len  = sizeof(my_struct_w);
	tot_required += iov_w[0].iov_len;

	iov_w[1].iov_base = &x_w;
	iov_w[1].iov_len  = sizeof(x_w);
	tot_required += iov_w[1].iov_len;

	iov_w[2].iov_base = str_w;
	iov_w[2].iov_len  = STR_SIZE;
	tot_required += iov_w[2].iov_len;

	num_written = my_writev(fd, iov_w, 3);

	if (num_written == -1)
		errx("writev");
	if (num_written < tot_required)
		printf("Written fewer bytes than requested\n");
	printf("total bytes requested: %ld; bytes written: %ld\n",
	       (long)tot_required, (long)num_written);

	close(fd);

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		errx("open");

	tot_required	  = 0;
	iov_r[0].iov_base = &my_struct_r;
	iov_r[0].iov_len  = sizeof(my_struct_r);
	tot_required += iov_r[0].iov_len;

	iov_r[1].iov_base = &x_r;
	iov_r[1].iov_len  = sizeof(x_r);
	tot_required += iov_r[1].iov_len;

	iov_r[2].iov_base = str_r;
	iov_r[2].iov_len  = STR_SIZE;
	tot_required += iov_r[2].iov_len;

	num_read = my_readv(fd, iov_r, 3);
	if (num_read == -1)
		errx("readv");
	if (num_read < tot_required)
		printf("Read fewer bytes than requested\n");
	printf("total bytes requested: %ld; bytes read: %ld\n",
	       (long)tot_required, (long)num_read);

	assert(((struct stat *)(iov_w[0].iov_base))->st_uid ==
	       ((struct stat *)(iov_r[0].iov_base))->st_uid);
	assert(((struct stat *)(iov_w[0].iov_base))->st_gid ==
	       ((struct stat *)(iov_r[0].iov_base))->st_gid);
	assert(((struct stat *)(iov_w[0].iov_base))->st_mode ==
	       ((struct stat *)(iov_r[0].iov_base))->st_mode);

	assert(x_w == x_r);
	assert(!strcmp(str_w, str_r));

	close(fd);
	//unlink(argv[1]);

	return 0;
}