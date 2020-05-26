#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include "common.h"
#include "error.h"

/* POSIX Issue 7, 2018 edition
 *  12. Utility Conventions - 12.2 Utility Syntax Guidelines - Guideline 1
 *  Utility names should be between two and nine characters, inclusive.
 */
#define APPNAMELEN 9

static char app_name[APPNAMELEN + 1];

void app_set_name(char *path)
{
	char *app_path, *bname;
	int path_len;

	if (app_name[0]) {
		warn("app_name already set to %s", app_name);
		return;
	}
	if (!path)
		errx("path cannot be NULL");

	path_len = strlen(path) + 1;
	app_path = malloc(path_len * sizeof(*path));
	if (!app_path)
		errx("malloc");
	strncpy(app_path, path, path_len);
	bname = basename(app_path);
	strncpy(app_name, bname, APPNAMELEN);
	free(app_path);
}

const char *app_get_name(void)
{
	if (!app_name[0])
		return "whoami?";
	return app_name;
}
