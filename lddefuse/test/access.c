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
	for(int i = 0; i < 10000; i++) {
		if(access(argv[1], F_OK) == -1) {
			fprintf(stderr, "Unable to access %s: %s\n", argv[1], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
