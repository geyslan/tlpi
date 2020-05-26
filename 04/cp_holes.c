#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"
#include "error.h"

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int nread;
	char *from, *to;
	int ifd, ofd;
	mode_t omode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	app_set_name(argv[0]);

	if (argc != 3)
		errx("Usage: %s from to\n", argv[0]);

	from = argv[1];
	to = argv[2];

	if ((ifd = open(from, O_RDONLY)) == -1)
		errx("open %s", from);

	if ((ofd = open(to, O_CREAT | O_TRUNC | O_WRONLY, omode)) == -1)
		errx("open %s", to);

	char buf[BUF_SIZE];
	while ((nread = read(ifd, buf, BUF_SIZE)) > 0) {
		for (int i = 0; i < nread; ++i) {
			int j = i;
			while (buf[j] == '\0' && j < nread)
				++j;
			if (lseek(ofd, j - i, SEEK_CUR) == -1)
				errx("lseek");
			if (j == nread)
				break;
			i = j;
			if (write(ofd, &buf[i], 1) != 1)
				errx("write %s", to);
		}
	}
	if (nread == -1)
		errx("read %s", from);

	if (close(ifd))
		errx("close %s", from);

	if (close(ofd))
		errx("close %s", to);

	return 0;
}
