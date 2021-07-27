#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
	if(argc != 2) {
		fprintf(stderr, "File path required\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < 10000; i++) {
		if(chmod(argv[1], S_IRUSR) == -1) {
			fprintf(stderr, "Unable to chmod %s: %s\n", argv[1], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
