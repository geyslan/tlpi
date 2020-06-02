#define _FILE_OFFSET_BITS 64

#include "common.h"
#include "error.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int   fd;
	off_t off;

	app_set_name(argv[0]);
	if (argc != 3 || strcmp(argv[1], "--help") == 0)
		errx("Usage: %s pathname offset\n", app_get_name());

	fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1)
		errx("open");
	off = atoll(argv[2]);
	if (lseek(fd, off, SEEK_SET) == -1)
		errx("lseek");
	if (write(fd, "test", 4) == -1)
		errx("write");

	return 0;
}