#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>

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

	while ((opt = getopt(argc, argv, "a")) != -1) {
		switch (opt) {
		case 'a':
			append = 1;
			break;
		default:
			fprintf(stderr, "Usage: %s -a file\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (append && optind >= argc) {
		fprintf(stderr, "Expected argument after option\n");
		exit(EXIT_FAILURE);
	}

	if (argc == 2) {
		output = argv[1];
	} else if (append) {
		output = argv[optind];
		oflags |= O_APPEND;
	}

	if (output && (outfd = open(output, oflags, omode)) == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	char buf[BUF_SIZE];
	while ((nread = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
		if (write(STDOUT_FILENO, buf, nread) == -1) {
			perror("write stdout");
			exit(EXIT_FAILURE);
		}

		if (outfd && (write(outfd, buf, nread) != nread)) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	}

	if (nread == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	if (outfd && close(outfd)) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	return 0;
}
