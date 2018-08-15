#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define LARGE_BUFFER_SIZE (128 * 1024 * 1024)
char buffer[LARGE_BUFFER_SIZE];

int main(int argc, char* argv[])
{
	if(argc != 2) {
		fprintf(stderr, "Usage: fusetestwrite size\n");
		exit(1);
	}
	int expected_size = atoi(argv[1]);
	int bytes_written = write(STDOUT_FILENO, buffer, expected_size);
	if(bytes_written == -1) {
		fprintf(stderr, "Write failed: %s\n", strerror(errno));
		exit(1);
	}
	if(bytes_written != expected_size) {
		fprintf(stderr, "Unexpected write size: %d expected %d\n", bytes_written, expected_size);
		exit(1);
	}
	return 0;
}
