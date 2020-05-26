#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>

void _err(const char *f, int ln, const char *fn, int toexit, int errnum,
		  const char *emsg, ...);

#define warn(fmt, args...) _err(__FILE__, __LINE__, __func__, 0, 0, fmt, ##args)
#define err(fmt, args...)                                                      \
	_err(__FILE__, __LINE__, __func__, 0, errno, fmt, ##args)
#define errx(fmt, args...)                                                     \
	_err(__FILE__, __LINE__, __func__, 1, errno, fmt, ##args)
#define errxno(errnum, fmt, args...)                                           \
	_err(__FILE__, __LINE__, __func__, 1, errnum, fmt, ##args)

#endif /* ERROR_H_ */
