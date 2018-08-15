#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define LARGE_BUFFER_SIZE (128 * 1024 * 1024)
char buffer[LARGE_BUFFER_SIZE];

int main(int argc, char* argv[])
{
	if(argc != 2) {
		fprintf(stderr, "Usage: fusetestread size\n");
		exit(1);
	}
	int expected_size = atoi(argv[1]);
	int bytes_read = read(STDIN_FILENO, buffer, expected_size);
	if(bytes_read == -1) {
		fprintf(stderr, "Read failed: %s\n", strerror(errno));
		exit(1);
	}
	if(bytes_read != expected_size) {
		fprintf(stderr, "Unexpected read size: %d expected %d\n", bytes_read, expected_size);
		exit(1);
	}
	return 0;
}
