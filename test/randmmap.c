#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>

#include "util.h"

static volatile char c;

int main(int argc, char* argv[])
{
	int fd = open(argv[1], O_RDWR);
	if(fd == -1) {
		errExit("open");
	}
	off_t file_size = get_file_size(fd);
	char* addr = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == MAP_FAILED) {
		errExit("mmap");
	}

	uint64_t start_time = get_time();
	int l = rand() % (file_size + 1);
	for(int i = 0; i < 1250000; i++) {
		c = addr[l];
		//addr[l] = l;
		l = rand() % (file_size + 1);
	}
	uint64_t end_time = get_time();

	printf("%" PRIu64 "\n", end_time - start_time);

	munmap(addr, file_size);
	close(fd);

	return 0;
}
