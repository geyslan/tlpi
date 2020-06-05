#include "common.h"
#include "error.h"
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd;

	app_set_name(argv[0]);
	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		errx("Usage: %s pathname\n", app_get_name());

	fd = open(argv[1], O_WRONLY | O_APPEND);
	if (fd == -1)
		errx("open %s", argv[1]);
	if (lseek(fd, 0, SEEK_SET) == -1)
		errx("lseek");
	if (write(fd, "TEST", 4) != 4)
		errx("write");
	if (close(fd) == -1)
		errx("close");

	return 0;
}