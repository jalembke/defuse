#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#include <string>

#define RANDOM_SEED 12345678

#define SMALL_BUFFER_SIZE 128
#define LARGE_BUFFER_SIZE (128 * 1024 * 1024)

char* target_dir = NULL;
int thread_count = 0;
int file_count = 0;
int do_read = 0;
char buffer[LARGE_BUFFER_SIZE];

int test_behavior = 0;
#define LARGE  0
#define SMALL  1
#define MIXED  2
#define RANDOM 3

void* thread_main(void* arg)
{
	std::string filepath = target_dir;
	filepath += "/";
	filepath += std::to_string(*(int*)(arg));
	for(int i = 0; i < file_count; i++) {
		int fd = -1;
		if(do_read != 0) {
			fd = open((filepath + std::to_string(i)).c_str(), O_RDONLY);
		} else {
			fd = open((filepath + std::to_string(i)).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		}
		if(fd > 0) {
			int expected_size = 0;
			switch(test_behavior) {
				case MIXED:
					expected_size = (i+1)*(LARGE_BUFFER_SIZE / (file_count+1));
					break;
				case LARGE:
					expected_size = LARGE_BUFFER_SIZE;
					break;
				case SMALL:
					expected_size = SMALL_BUFFER_SIZE;
					break;
				case RANDOM:
					expected_size = (rand() % (LARGE_BUFFER_SIZE - 1)) + 1;
					break;
			}
			//printf("%d %d\n", i, expected_size);
			int ret = -1;
			if(do_read != 0) {
				ret = read(fd, buffer, expected_size);
			} else {
				ret = write(fd, buffer, expected_size);
			}
			if(ret == -1) {
				fprintf(stderr, "Unable to perform op to %s: %s\n", (filepath + std::to_string(i)).c_str(), strerror(errno));
				exit(1);
			}
			if(ret != expected_size) {
				fprintf(stderr, "Op to file %s size: %d expected %d\n", (filepath + std::to_string(i)).c_str(), ret, expected_size);
				exit(1);
			}
		} else {
			fprintf(stderr, "Unable to open %s: %s\n", (filepath + std::to_string(i)).c_str(), strerror(errno));
			exit(1);
		}
		close(fd);
	}
	return NULL;
}

#define NSEC_PER_SEC 1000000000
static inline uint64_t getTime() 
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
	pthread_t* thread_ids = NULL;
	int* thread_args = NULL;
	uint64_t start_time = 0;
	uint64_t end_time = 0;
	int argi = 1;

	if(argc != 6) {
		printf("Usage: fusetest r|w l|s|m|r target_dir threads files\n");
		exit(1);
	}

	if(argv[argi][0] == 'r') {
		do_read = 1;
	} else if(argv[argi][0] == 'w') {
		do_read = 0;
	} else {
		printf("invalid test type: %s\n", argv[argi]);
		exit(1);
	}
	argi++;

	if(argv[argi][0] == 'l') {
		test_behavior = LARGE;
	} else if(argv[argi][0] == 'm') {
		test_behavior = MIXED;
	} else if(argv[argi][0] == 's') {
		test_behavior = SMALL;
	}  else if(argv[argi][0] == 'r') {
		test_behavior = RANDOM;
	} else {
		printf("invalid test behavior: %s\n", argv[argi]);
		exit(1);
	}
	argi++;

	target_dir = (char*)argv[argi++];
	thread_count = atoi(argv[argi++]);
	file_count = atoi(argv[argi++]);

	if(thread_count <= 0) {
		printf("threads must be > 0\n");
		exit(1);
	}
	if(file_count <= 0) {
		printf("files must be > 0\n");
		exit(1);
	}

	thread_ids = new pthread_t[thread_count];
	thread_args = new int[thread_count];

	if(test_behavior == RANDOM) {
		srand(RANDOM_SEED);
	}

	start_time = getTime();
	for(int i = 0; i < thread_count; i++) {
		thread_args[i] = i;
		if(pthread_create(&thread_ids[i], NULL, thread_main, &(thread_args[i])) != 0) {
			printf("Create thread %d failed\n", i);
			exit(1);
		}
	}
	for(int i = 0; i < thread_count; i++) {
		pthread_join(thread_ids[i], NULL);
	}
	end_time = getTime();

	printf("%" PRIu64 "\n", end_time - start_time);

	delete [] thread_ids;
	delete [] thread_args;

	return 0;
}
