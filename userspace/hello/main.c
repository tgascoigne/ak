#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
	fork();
	printf("Hello, userspace! My pid is %i\n", getpid());
	return 0;
}
