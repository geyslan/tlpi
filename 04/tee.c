#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "common.h"
#include "error.h"

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int opt;
	int append = 0;
	int nread;
	char *output = NULL;
	int outfd = 0;
	int oflags = O_CREAT | O_WRONLY;
	mode_t omode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	app_set_name(argv[0]);

	while ((opt = getopt(argc, argv, "a")) != -1) {
		switch (opt) {
		case 'a':
			append = 1;
			break;
		default:
			errx("Usage: %s -a file", argv[0]);
		}
	}

	if (append && optind >= argc)
		errx("Expected argument after option");

	if (argc == 2) {
		output = argv[1];
	} else if (append) {
		output = argv[optind];
		oflags |= O_APPEND;
	}

	if (output && (outfd = open(output, oflags, omode)) == -1)
		errx("open");

	char buf[BUF_SIZE];
	while ((nread = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
		if (write(STDOUT_FILENO, buf, nread) == -1)
			errx("write stdout");

		if (outfd && (write(outfd, buf, nread) != nread))
			errx("write");
	}

	if (nread == -1)
		errx("read");

	if (outfd && close(outfd))
		errx("close");

	return 0;
}
