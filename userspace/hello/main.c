#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
	printf("Hello, userspace! My pid is %i, and my parent is %i\n", getpid(), getppid());
	return 0;
}
