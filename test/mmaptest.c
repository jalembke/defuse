#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int fd = open("/tmp/usfsal/test", O_RDWR);
	if(fd == -1) {
		fprintf(stderr, "Can't open: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	void* addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == MAP_FAILED) {
		fprintf(stderr, "Can't mmap: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int i = ((int*)addr)[10];
	
	close(fd);

	return 0;
}
