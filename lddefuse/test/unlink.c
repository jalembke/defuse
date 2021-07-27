#include <fcntl.h>
#include <unistd.h>
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
	char crtbuf[64];
	for(int i = 0; i < 10000; i++) {
		snprintf(crtbuf, sizeof(crtbuf), "%s%d", argv[1], i);
		if(unlink(crtbuf) == -1) {
			fprintf(stderr, "Unable to unlink %s: %s\n", crtbuf, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
