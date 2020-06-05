#include "common.h"
#include "error.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int	  fd;
	int	  append = 1;
	int	  flag	 = O_CREAT | O_WRONLY;
	mode_t	  mode	 = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	long long size	 = 0;

	app_set_name(argv[0]);
	if (argc < 3 || strcmp(argv[1], "--help") == 0)
		errx("Usage: %s filename num-bytes [x]", app_get_name());
	size = atoll(argv[2]);
	if (argc == 4 && argv[3] == "x")
		append = 0;
	flag |= append ? O_APPEND : 0;

	fd = open(argv[1], flag, mode);
	if (fd == -1)
		errx("open %s", argv[1]);
	while (size) {
		if (!append && (lseek(fd, 0, SEEK_END) == -1))
			errx("lseek");
		if (write(fd, "B", 1) != 1)
			errx("write");
		--size;
	}
	if (close(fd) == -1)
		errx("close");

	return 0;
}