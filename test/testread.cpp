#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define LARGE_BUFFER_SIZE (128 * 1024 * 1024)
char buffer[LARGE_BUFFER_SIZE];

int main(int argc, char* argv[])
{
	if(argc != 6) {
		fprintf(stderr, "Usage: fusetestread size file_count first_fd fd_inc size_inc\n");
		exit(1);
	}
	int expected_size = atoi(argv[1]);
	int file_count = atoi(argv[2]);
	int first_fd = atoi(argv[3]);
	int fd_inc = atoi(argv[4]);
	int size_inc = atoi(argv[5]);
	int fd = first_fd;
	for(int i = 0; i < file_count; i++) {
		int bytes_read = read(fd, buffer, expected_size);
		if(bytes_read == -1) {
			fprintf(stderr, "Read failed %d: %s\n", fd, strerror(errno));
			exit(1);
		}
		if(bytes_read != expected_size) {
			fprintf(stderr, "Unexpected read size: %d expected %d\n", bytes_read, expected_size);
			exit(1);
		}
		fd += fd_inc;
		expected_size += size_inc;
	}
	return 0;
}
