#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char* argv[])
{
	char readbuf[PATH_MAX];
	if(argc != 2) {
		fprintf(stderr, "File path required\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < 10000; i++) {
		if(readlink(argv[1], readbuf, PATH_MAX) == -1) {
			fprintf(stderr, "Unable to readlink %s: %s\n", argv[1], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
