#ifndef COMMON_H_
#define COMMON_H_

#define _POSIX_C_SOURCE 200809L

#define STR(x)	     #x
#define STRINGIZE(x) STR(x)
#define min(x, y)    (((x) < (y)) ? (x) : (y))

void	    app_set_name(char *path);
const char *app_get_name();

#endif /* COMMON_H_ */
