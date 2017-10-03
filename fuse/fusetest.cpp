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

#define BUFFER_SIZE (512 * 1024)

char* target_dir = NULL;
int thread_count = 0;
int file_count = 0;

void* write_main(void* arg)
{
	char write_buffer[BUFFER_SIZE];
	std::string filepath = target_dir;
	filepath += "/testfile";
	filepath += std::to_string(*(int*)(arg));
	for(int i = 0; i < file_count; i++) {
		int fd = open((filepath + std::to_string(i)).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if(fd > 0) {
			int ret = write(fd, write_buffer, sizeof(write_buffer));
			if(ret == -1) {
				fprintf(stderr, "Unable to write to %s: %s\n", (filepath + std::to_string(i)).c_str(), strerror(errno));
				exit(1);
			}
			if(ret != sizeof(write_buffer)) {
				fprintf(stderr, "Write to file %s size: %d expected %d\n", (filepath + std::to_string(i)).c_str(), ret, (int)sizeof(write_buffer));
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

void* read_main(void* arg)
{
	char read_buffer[BUFFER_SIZE];
	std::string filepath = target_dir;
	filepath += "/testfile";
	filepath += std::to_string(*(int*)(arg));
	for(int i = 0; i < file_count; i++) {
		int fd = open((filepath + std::to_string(i)).c_str(), O_RDONLY);
		if(fd > 0) {
			int ret = read(fd, read_buffer, sizeof(read_buffer));
			if(ret == -1) {
				fprintf(stderr, "Unable to read to %s: %s\n", (filepath + std::to_string(i)).c_str(), strerror(errno));
				exit(1);
			}
			if(ret != sizeof(read_buffer)) {
				fprintf(stderr, "Read from file %s size: %d expected %d\n", (filepath + std::to_string(i)).c_str(), ret, (int)sizeof(read_buffer));
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
	pthread_t* thread_ids = NULL;
	int* thread_args = NULL;
	uint64_t start_time = 0;
	uint64_t end_time = 0;

	if(argc != 5) {
		printf("Usage: fusetest r|w target_dir threads files\n");
		exit(1);
	}
	target_dir = (char*)argv[2];
	thread_count = atoi(argv[3]);
	file_count = atoi(argv[4]);

	if(thread_count <= 0) {
		printf("threads must be > 0\n");
		exit(1);
	}
	if(file_count <= 0) {
		printf("files must be > 0\n");
		exit(1);
	}
	void *(*thread_func)(void*) = NULL;
	if(argv[1][0] == 'r') {
		thread_func = read_main;
	}
	else if(argv[1][0] == 'w') {
		thread_func = write_main;
	} else {
		printf("invalid test type: %s\n", argv[1]);
		exit(1);
	}

	thread_ids = new pthread_t[thread_count];
	thread_args = new int[thread_count];

	start_time = getTime();
	for(int i = 0; i < thread_count; i++) {
		thread_args[i] = i;
		if(pthread_create(&thread_ids[i], NULL, thread_func, &(thread_args[i])) != 0) {
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
