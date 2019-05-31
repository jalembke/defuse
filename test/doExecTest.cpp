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
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string>

#define CHILD_EXE "/doCat"

char* target_dir = NULL;
int file_count = 0;

char child_exec_path[PATH_MAX + 1];

static inline char* get_path(char* path, size_t size)
{
	if(-1 == readlink("/proc/self/exe", path, size)) {
		fprintf(stderr, "Failed to read /proc/self/exe: %s\n", strerror(errno));
		exit(1);
	}
	char* last_slash = strrchr(path, '/');
	if(last_slash) {
		(*last_slash) = 0;
	}
	return path;
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
	if(argc != 3) {
		printf("Usage: doExecTest target_dir file_count\n");
		exit(1);
	}

	int argi = 1;
	int* fds = NULL;
	uint64_t start_time = 0;
	uint64_t end_time = 0;
	
	target_dir = argv[argi++];
	file_count = atoi(argv[argi++]);

	if(file_count <= 0) {
		printf("files must be > 0: %d\n", file_count);
		exit(1);
	}

	char exe_path[PATH_MAX+1];
	get_path(exe_path, sizeof(exe_path));
	memset(child_exec_path, 0x00, sizeof(child_exec_path));
	strcat(child_exec_path, exe_path);
	strcat(child_exec_path, CHILD_EXE);

	std::string base_file_path = target_dir;
	fds = new int[file_count];
	for(int i = 0; i < file_count; i++) {
		std::string filepath = base_file_path + "/" + std::to_string(i);
		fds[i] = open(filepath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		if(fds[i] == -1) {
			fprintf(stderr, "Unable to open %s: %s\n", filepath.c_str(), strerror(errno));
			exit(1);
		}
	}

	start_time = getTime();
	pid_t child_pid = fork();
	if(-1 == child_pid) {
		fprintf(stderr, "Unable for fork: %s\n", strerror(errno));
		exit(1);
	}
	if(child_pid == 0) {
		dup2(fds[0], STDIN_FILENO);
		char* const parm_list[] = {child_exec_path, NULL};
		if(-1 == execv(child_exec_path, parm_list)) {
			fprintf(stderr, "Unable to exec %s in child: %s\n", child_exec_path, strerror(errno));
			exit(1);
		}
	} else {
		int status;
		waitpid(child_pid, &status, 0);
		if(!WIFEXITED(status)) {
			fprintf(stderr, "Child process %d changed status without exiting\n", child_pid);
			exit(1);
		}
		if(WEXITSTATUS(status) != 0) {
			fprintf(stderr, "Child process %d exited with exit code: %d\n", child_pid, WEXITSTATUS(status));
			exit(1);
		}
	}
	end_time = getTime();

	printf("%" PRIu64 "\n", end_time - start_time);
	
	for(int i = 0; i < file_count; i++) {
		close(fds[i]);	
	}
	delete [] fds;

	return 0;
}
