#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "common.h"
#include "error.h"

#define ERRBUFLEN_ 500

static void _pr(const char *f, int ln, const char *fn, int errnum,
				const char *emsg, va_list ap)
{
	const char *app_name = app_get_name();
	char buf[ERRBUFLEN_ + 1];

#ifdef DEBUG
	fprintf(stderr, "[%s:%d - %s()] ", f, ln, fn);
#endif
	vsnprintf(buf, ERRBUFLEN_, emsg, ap);
	if (errnum)
		fprintf(stderr, "%s: %s [%d: %s]\n", app_name, buf, errnum,
				strerror(errnum));
	else
		fprintf(stderr, "%s: %s\n", app_name, buf);
}

void _err(const char *f, int ln, const char *fn, int toexit, int errnum,
		  const char *emsg, ...)
{
	int olderrno = errno;

	va_list ap;
	va_start(ap, emsg);
	_pr(f, ln, fn, errnum, emsg, ap);
	va_end(ap);

	if (toexit) {
		if (!errnum)
			errnum = EXIT_FAILURE;
		exit(errnum);
	}

	errno = olderrno;
}