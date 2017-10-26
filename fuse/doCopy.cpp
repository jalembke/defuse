#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#include <string>

#define NSEC_PER_SEC 1000000000
uint64_t getTime() 
{
	struct timespec ts;
	if(clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		fprintf(stderr, "Unable to retrieve current time: %s\n", strerror(errno));
		exit(1);
	}
	return ts.tv_sec * NSEC_PER_SEC + ts.tv_nsec;
}
#undef NSEC_PER_SEC

int main(int argc, char* argv[])
{
	if(argc != 4) {
		printf("Usage: doCopy srcdir tgtdir count\n");
		exit(1);
	}

	std::string src = argv[1];
	src += "/0";
	std::string tgt = argv[2];
	tgt += "/0";
	int file_count = atoi(argv[3]);

	char buffer[32768];

	uint64_t start_time = getTime();
	for(int i = 0; i < file_count; i++) {
		int srcfd = open((src + std::to_string(i)).c_str(), O_RDONLY);
		if(srcfd == -1) {
			fprintf(stderr, "Unable to open %s: %s\n", (src + std::to_string(i)).c_str(), strerror(errno));
			exit(1);
		}
		int tgtfd = open((tgt + std::to_string(i)).c_str(), O_WRONLY|O_CREAT|O_EXCL, 0600);
		if(tgtfd == -1) {
			fprintf(stderr, "Unable to open %s: %s\n", (tgt + std::to_string(i)).c_str(), strerror(errno));
			exit(1);
		}

		int size_read = read(srcfd, buffer, sizeof(buffer));
		if(size_read == -1) {
			fprintf(stderr, "Unable to read to %s: %s\n", (src + std::to_string(i)).c_str(), strerror(errno));
			exit(1);
		}
		if(write(tgtfd, buffer, size_read) == -1) {
			fprintf(stderr, "Unable to write to %s: %s\n", (tgt + std::to_string(i)).c_str(), strerror(errno));
			exit(1);
		}
		close(srcfd);
		close(tgtfd);
	}
	uint64_t end_time = getTime();
	printf("%" PRIu64 "\n", end_time - start_time);
}
