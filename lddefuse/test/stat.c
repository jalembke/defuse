#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[])
{
	if(argc != 2) {
		fprintf(stderr, "File path required\n");
		exit(EXIT_FAILURE);
	}
	struct stat statbuf;
	for(int i = 0; i < 1; i++) {
		if(stat(argv[1], &statbuf) == -1) {
			fprintf(stderr, "Unable to stat %s: %s\n", argv[1], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
