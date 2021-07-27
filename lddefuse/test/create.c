#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if(argc != 2) {
		fprintf(stderr, "File path required\n");
		exit(EXIT_FAILURE);
	}
	char crtbuf[64];
	for(int i = 0; i < 10000; i++) {
		snprintf(crtbuf, sizeof(crtbuf), "%s%d", argv[1], i);
		int fd = creat(crtbuf, S_IRUSR | S_IWUSR);
		if(fd == -1) {
			fprintf(stderr, "Unable to create %s: %s\n", crtbuf, strerror(errno));
			exit(EXIT_FAILURE);
		}
		close(fd);
	}
	return 0;
}
