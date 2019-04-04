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

#include "util.h"
#define CHILD_EXE "doCat"

int main(int argc, char* argv[])
{
	char child_exec_path[PATH_MAX + 1];
	
	if(argc != 3) {
		printf("Usage: doExecTest target_dir file_count\n");
		exit(EXIT_FAILURE);
	}

	int argi = 1;
	int* fds = NULL;
	uint64_t start_time = 0;
	uint64_t end_time = 0;
	
	char* target_dir = argv[argi++];
	int file_count = atoi(argv[argi++]);

	if(file_count <= 0) {
		printf("files must be > 0: %d\n", file_count);
		exit(EXIT_FAILURE);
	}

	char exe_path[PATH_MAX+1];
	get_path(exe_path, PATH_MAX);
	memset(child_exec_path, 0x00, sizeof(child_exec_path));
	snprintf(child_exec_path, PATH_MAX, "%s/%s", exe_path, CHILD_EXE);

	char file_path[PATH_MAX+1];
	fds = (int*)malloc(file_count * sizeof(int));
	for(int i = 0; i < file_count; i++) {
		snprintf(file_path, PATH_MAX, "%s/%d", target_dir, i);
		fds[i] = open(file_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		if(fds[i] == -1) {
			fprintf(stderr, "Unable to open %s: %s\n", file_path, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	start_time = get_time();
	pid_t child_pid = fork();
	if(-1 == child_pid) {
		fprintf(stderr, "Unable for fork: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(child_pid == 0) {
		dup2(fds[0], STDIN_FILENO);
		char* const parm_list[] = {child_exec_path, NULL};
		if(-1 == execv(child_exec_path, parm_list)) {
			fprintf(stderr, "Unable to exec %s in child: %s\n", child_exec_path, strerror(errno));
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
	end_time = get_time();

	printf("%" PRIu64 "\n", end_time - start_time);
	
	for(int i = 0; i < file_count; i++) {
		close(fds[i]);	
	}
	free(fds);

	return 0;
}
