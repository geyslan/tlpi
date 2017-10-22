#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int nread;
	char *from, *to;
	int ifd, ofd;
	mode_t omode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s from to\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	from = argv[1];
	to = argv[2];

	if ((ifd = open(from, O_RDONLY)) == -1) {
		perror("open from");
		exit(EXIT_FAILURE);
	}

	if ((ofd = open(to, O_CREAT | O_TRUNC | O_WRONLY, omode)) == -1) {
		perror("open to");
		exit(EXIT_FAILURE);
	}

	char buf[BUF_SIZE];
	while ((nread = read(ifd, buf, BUF_SIZE)) > 0) {
		for (int i = 0; i < nread; ++i) {
			if (buf[i] == '\0') {
				if (lseek(ofd, 1, SEEK_CUR) == -1) {
					perror("lseek");
					exit(EXIT_FAILURE);
				}
			} else if (write(ofd, &buf[i], 1) != 1) {
				perror("write");
				exit(EXIT_FAILURE);
			}
		}
	}
	if (nread == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	if (close(ifd)) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	if (close(ofd)) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	return 0;
}
