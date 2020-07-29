#include <error.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct passwd *my_getpwnam(const char *name)
{
	struct passwd *pwd;

	if (!name)
		return NULL;

	setpwent();
	while ((pwd = getpwent())) {
		if (strcmp(pwd->pw_name, name) == 0)
			break;
	}
	endpwent();

	return pwd;
}

int main(int argc, char *argv[])
{
	struct passwd *pwd;

	if (argc < 2) {
		fprintf(stderr, "Usage: getpwnam 'username'\n");
		exit(EXIT_FAILURE);
	}

	pwd = my_getpwnam(argv[1]);

	if (!pwd) {
		printf("User '%s' not found.\n", argv[1]);
	} else {
		printf("Name:  %s\n", pwd->pw_name);
		printf("Pass:  %s\n", pwd->pw_passwd);
		printf("UID:   %u\n", pwd->pw_uid);
		printf("GID:   %u\n", pwd->pw_gid);
		printf("Gecos: %s\n", pwd->pw_gecos);
		printf("Home:  %s\n", pwd->pw_dir);
		printf("Shell: %s\n", pwd->pw_shell);
	}

	return 0;
}