#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	if(argc != 2) {
		fprintf(stderr, "File path required\n");
		exit(EXIT_FAILURE);
	}
	int fd = openat(AT_FDCWD, argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(fd == -1) {
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}
	close(fd);
	return 0;
}
