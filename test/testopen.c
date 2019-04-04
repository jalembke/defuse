#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define BASE_DIR "/tmp/tst_open%d"

int main(int argc, char* argv[])
{
	char file_buffer[1024];

	if(argc != 2) {
		fprintf(stderr, "Usage: testopen file_count\n");
		exit(1);
	}
	int file_count = atoi(argv[1]);
	for(int i = 0; i < file_count; i++) {
		snprintf(file_buffer, sizeof(file_buffer), BASE_DIR, i);
		int fd = open(file_buffer, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
		if (fd == -1) {
			fprintf(stderr, "Unable to open %s: %s\n", file_buffer, strerror(errno));
			exit(1);
		}
		close(fd);
	}
	return 0;
}
