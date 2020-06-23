#include <setjmp.h>
#include <stdio.h>

jmp_buf env;

void jumpingin(int arg)
{
	int a = 10;
	int b = 20;

	if (arg)
		longjmp(env, 1);
}

void jumpedout(void)
{
	int a	= 25;
	char *s = "Hello";

	switch (setjmp(env)) {
	case 0:
		printf("Setting IP for return!\n");
		return;
	case 1:
		printf("%s [%d]! I went back!\n", s, a);
		break;
	}

	printf("Returning to where???\n");
}

int main(int argc)
{
	jumpedout();
	printf("After returning!\n");
	jumpingin(argc);

	return 0;
}
