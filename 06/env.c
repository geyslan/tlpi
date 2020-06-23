#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int my_setenv(const char *name, const char *value, int overwrite)
{
	char *oldval;
	char *newvar;
	int varlen = 0;

	if (!name || *name == '\0' || strchr(name, '='))
		return EINVAL;

	oldval = getenv(name);
	if (oldval) {
		if (!overwrite || strcmp(oldval, value) == 0)
			return 0;
		// remove variable
		putenv((char *) name);
		if (!value)
			return 0;
	}
	
	// VARNAME + '=' + VARVALUE + NUL
	varlen = strlen(name) + 1 + strlen(value) + 1;
	newvar = malloc(varlen);
	if (!newvar)
		return ENOMEM;

	strcat(newvar, name);
	strcat(newvar, "=");
	strcat(newvar, value);
	
	return putenv(newvar);
}

int main(int argc, char *argv[])
{
	int overwrite = 0;
	int ret = 0;

	if (argc < 3)
		return 0;

	if (argc > 3)
		overwrite = 1;
	
	printf("Testing my_setenv()\n");
	printf(" input: %s=%s (over=%d)\n", argv[1], argv[2], overwrite);
	ret = my_setenv(argv[1], argv[2], overwrite);	

	errno = ret;
	perror(" status");
	printf(" %s=%s\n", argv[1], getenv(argv[1]));

	return ret;
}