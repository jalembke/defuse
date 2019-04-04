#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MYFILE "/tmp/usfsal/tmp/test.tar.gz#/test/testfile00"

int main(int argc, char* argv[])
{
	char buffer[1024];
	int fd = open(MYFILE, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open %s: %s\n", MYFILE, strerror(errno));
		exit(1);
	}
	ssize_t bytes_read = read(fd, &buffer, 1024);
	if(bytes_read < 0) {
		fprintf(stderr, "Unable to read %s: %s\n", MYFILE, strerror(errno));
		exit(1);
	}
	printf("BYTES READ: %ld\n", bytes_read);
	close(fd);
	return 0;
}
