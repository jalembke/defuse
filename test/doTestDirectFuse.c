#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "util.h"
#include "sysio.h"

#define RANDOM_SEED 12345678

#define VERY_SMALL_BUFFER_SIZE 128
#define SMALL_BUFFER_SIZE 14129
#define LARGE_BUFFER_SIZE (128 * 1024 * 1024)

char* target_dir = NULL;
char* backend_dir = NULL;
int thread_count = 0;
int file_count = 0;
int do_read = 0;
char buffer[LARGE_BUFFER_SIZE];

bool with_fork = false;
char child_read_path[PATH_MAX + 1];
char child_write_path[PATH_MAX + 1];

int test_behavior = 0;
#define LARGE  0
#define SMALL  1
#define VERY_SMALL 2
#define MIXED  3
#define RANDOM 4

int mixed_increment = 0;

static inline int get_expected_size(int i) 
{
	int expected_size = 0;
	switch(test_behavior) {
		case MIXED:
			expected_size = (i+1)*(mixed_increment);
			break;
		case LARGE:
			expected_size = LARGE_BUFFER_SIZE;
			break;
		case SMALL:
			expected_size = SMALL_BUFFER_SIZE;
			break;
		case VERY_SMALL:
			expected_size = VERY_SMALL_BUFFER_SIZE;
			break;
		case RANDOM:
			expected_size = (rand() % (LARGE_BUFFER_SIZE - 1)) + 1;
			break;
	}
	//printf("%d %d\n", i, expected_size);
	return expected_size;
}

static inline void do_op_with_fork(const char* base_file_path)
{
	char file_path[PATH_MAX];
	int* fds = (int*)malloc(file_count * sizeof(int));
	for(int i = 0; i < file_count; i++) {
		snprintf(file_path, PATH_MAX, "bbfs:%s%d", base_file_path, i);
		int fd = -1;
		if(do_read != 0) {
			fd = SYSIO_INTERFACE_NAME(open)(file_path, O_RDONLY);
		} else {
			fd = SYSIO_INTERFACE_NAME(open)(file_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		}
		if(fd == -1) {
			fprintf(stderr, "%d: Unable to open %s: %s\n", do_read, file_path, strerror(errno));
			exit(EXIT_FAILURE);
		}
		fds[i] = fd;
	}
	pid_t child_pid = fork();
	if(-1 == child_pid) {
		fprintf(stderr, "Unable for fork: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(child_pid == 0) {
		char* cmd;
		int expected_size = get_expected_size(0);
		char expected_size_parm[128];
		snprintf(expected_size_parm, sizeof(expected_size_parm), "%d", expected_size);
		char file_count_parm[128];
		snprintf(file_count_parm, sizeof(file_count_parm), "%d", file_count);
		char file_desc_parm[128];
		snprintf(file_desc_parm, sizeof(file_desc_parm), "%d", fds[0]);
		char file_desc_inc_parm[128];
		if(file_count > 1) {
			snprintf(file_desc_inc_parm, sizeof(file_desc_parm), "%d", fds[1] - fds[0]);
		} else {
			snprintf(file_desc_inc_parm, sizeof(file_desc_inc_parm), "%d", 0);
		}
		char size_inc_parm[128];
		if(test_behavior == MIXED) {
			snprintf(size_inc_parm, sizeof(size_inc_parm), "%d", mixed_increment);
		} else {
			snprintf(size_inc_parm, sizeof(size_inc_parm), "%d", 0);
		}

		if(do_read != 0) {
			cmd = child_read_path;
		} else {
			cmd = child_write_path;
		}
		char* const parm_list[] = {cmd, expected_size_parm, file_count_parm, file_desc_parm, file_desc_inc_parm, size_inc_parm, NULL};
		if(-1 == execv(cmd, parm_list)) {
			fprintf(stderr, "Unable to exec %s in child: %s\n", cmd, strerror(errno));
			exit(EXIT_FAILURE);
		}
	} else {
		int status;
		waitpid(child_pid, &status, 0);
		if(!WIFEXITED(status)) {
			fprintf(stderr, "Child process %d changed status without exiting\n", child_pid);
			exit(EXIT_FAILURE);
		}
		if(WEXITSTATUS(status) != 0) {
			fprintf(stderr, "Child process %d exited with exit code: %d\n", child_pid, WEXITSTATUS(status));
			exit(EXIT_FAILURE);
		}
	}
	for(int i = 0; i < file_count; i++) {
		SYSIO_INTERFACE_NAME(close)(fds[i]);
	}
	free(fds);
}

static inline void do_op_within_proc(const char* base_file_path)
{
	int ret = -1;
	char file_path[PATH_MAX];
	for(int i = 0; i < file_count; i++) {
		snprintf(file_path, PATH_MAX, "bbfs:%s%d", base_file_path, i);
		int fd = -1;
		if(do_read != 0) {
			fd = SYSIO_INTERFACE_NAME(open)(file_path, O_RDONLY);
		} else {
			fd = SYSIO_INTERFACE_NAME(open)(file_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		}
		if(fd == -1) {
			fprintf(stderr, "%d: Unable to open %s: %s\n", do_read, file_path, strerror(errno));
			exit(EXIT_FAILURE);
		}
		int expected_size = get_expected_size(i);
		if(do_read != 0) {
			ret = SYSIO_INTERFACE_NAME(pread)(fd, buffer, expected_size, 0);
		} else {
			ret = SYSIO_INTERFACE_NAME(pwrite)(fd, buffer, expected_size, 0);
			// fsync(fd);
		}
		if(ret == -1) {
			fprintf(stderr, "Unable to perform op to fd %d: %s\n", fd, strerror(errno));
			exit(1);
		}
		if(ret != expected_size) {
			fprintf(stderr, "Op to fd %d size: %d expected %d\n", fd, ret, expected_size);
			exit(1);
		}
		SYSIO_INTERFACE_NAME(close)(fd);
	}
}

void* thread_main(void* arg)
{
	char base_file_path[PATH_MAX];
	snprintf(base_file_path, PATH_MAX, "%s/%d", target_dir, *(int*)(arg));
	if(with_fork) {
		do_op_with_fork(base_file_path);
	} else {
		do_op_within_proc(base_file_path);
	}
}

int main(int argc, char* argv[])
{
	pthread_t* thread_ids = NULL;
	int* thread_args = NULL;
	uint64_t start_time = 0;
	uint64_t end_time = 0;
	int argi = 1;
	int err = 0;

	if(argc != 7 && argc != 8) {
		printf("Usage: doTest [f] r|w l|s|v|m|r target_dir backend_dir threads files\n");
		exit(1);
	}

	if(argv[argi][0] == 'f') {
		with_fork = true;
		argi++;
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
	} else if(argv[argi][0] == 'v') {
		test_behavior = VERY_SMALL;
	}  else if(argv[argi][0] == 'r') {
		test_behavior = RANDOM;
	} else {
		printf("invalid test behavior: %s\n", argv[argi]);
		exit(1);
	}
	argi++;

	target_dir = (char*)argv[argi++];
	backend_dir = (char*)argv[argi++];
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

	err = SYSIO_INTERFACE_NAME(mount)(backend_dir, target_dir, "bbfs", 2, NULL);
	if(err) {
		fprintf(stderr, "BBFS mount failed\n");
		exit(1);
	}

	thread_ids = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
	thread_args = (int*)malloc(thread_count * sizeof(int));

	if(test_behavior == RANDOM) {
		srand(RANDOM_SEED);
	}

	if(test_behavior == MIXED) {
		mixed_increment = LARGE_BUFFER_SIZE / ((file_count * (file_count+1)) / 2);
	}

	char exe_path[PATH_MAX+1];
	get_path(exe_path, sizeof(exe_path));
	memset(child_read_path, 0x00, sizeof(child_read_path));
	strcat(child_read_path, exe_path);
	strcat(child_read_path, "/testread");
	memset(child_write_path, 0x00, sizeof(child_write_path));
	strcat(child_write_path, exe_path);
	strcat(child_write_path, "/testwrite");

	start_time = get_time();
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
	sync();
	end_time = get_time();

	printf("%" PRIu64 "\n", end_time - start_time);

	char umount_path[PATH_MAX+1];
	snprintf(umount_path, sizeof(umount_path), "bbfs:%s", target_dir);
	SYSIO_INTERFACE_NAME(umount)(target_dir);

	free(thread_ids);
	free(thread_args);

	return 0;
}
