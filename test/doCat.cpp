#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	char buffer[32768];
	ssize_t size_read = read(STDIN_FILENO, buffer, sizeof(buffer));
	if(size_read == -1) {
		fprintf(stderr, "Unable to read from STDIN: %s\n", strerror(errno));
		exit(1);
	}
	if(-1 == write(STDOUT_FILENO, buffer, size_read)) {
		fprintf(stderr, "Unable to write to STDOUT: %s\n", strerror(errno));
		exit(1);
	}
	return 0;
}
