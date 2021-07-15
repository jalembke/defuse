#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[])
{
	char crtbuf[64];
	for(int i = 0; i < 10000; i++) {
		snprintf(crtbuf, sizeof(crtbuf), "/tmp/fuse/mnt/test/test%d", i);
		int fd = creat(crtbuf, S_IRUSR | S_IWUSR);
		if(fd == -1) {
			printf("ERROR: %s\n", strerror(errno));
			return 1;
		}
		close(fd);
	}
	return 0;
}
