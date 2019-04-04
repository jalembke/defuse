#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>
#include <sys/wait.h>

#include "util.h"

int main(int argc, char* argv[])
{
	char src_path[PATH_MAX+1];
	char tgt_path[PATH_MAX+1];
	char cat_path[PATH_MAX+1];

	if(argc != 4 && argc != 5) {
		printf("Usage: doCopy srcdir tgtdir count\n");
		exit(1);
	}

	char* src = argv[1];
	char* tgt = argv[2];
	int file_count = atoi(argv[3]);

	bool with_fork = false;
	pid_t child_pid = 0;
	if (argc == 5) {
		char pwd_path[PATH_MAX+1];
		get_path(pwd_path, PATH_MAX);
		snprintf(cat_path, PATH_MAX, "%s/doCat", pwd_path);
		with_fork = true;
	}

	char buffer[32768];

	uint64_t start_time = get_time();
	for(int i = 0; i < file_count; i++) {
		snprintf(src_path, PATH_MAX, "%s/0%d", src, i);
		int srcfd = open(src_path, O_RDONLY);
		if(srcfd == -1) {
			fprintf(stderr, "Unable to open %s: %s\n", src_path, strerror(errno));
			exit(1);
		}
		snprintf(tgt_path, PATH_MAX, "%s/0%d", tgt, i);
		int tgtfd = open(tgt_path, O_WRONLY|O_CREAT|O_EXCL, 0600);
		if(tgtfd == -1) {
			fprintf(stderr, "Unable to open %s: %s\n", tgt_path, strerror(errno));
			exit(1);
		}

		if(with_fork) {
			child_pid = fork();
			if(-1 == child_pid) {
				fprintf(stderr, "Unable for fork: %s\n", strerror(errno));
				exit(1);
			}
			if(child_pid == 0) {
				dup2(srcfd, STDIN_FILENO);
				dup2(tgtfd, STDOUT_FILENO);
				close(srcfd);
				close(tgtfd);
				char* const parm_list[] = {(char*)cat_path, NULL};
				if(-1 == execv(cat_path, parm_list)) {
					fprintf(stderr, "Unable to exec %s in child: %s\n", cat_path, strerror(errno));
					exit(1);
				}
			} else {
				waitpid(child_pid, NULL, 0);
			}
		} else {
			int size_read = read(srcfd, buffer, sizeof(buffer));
			if(size_read == -1) {
				fprintf(stderr, "Unable to read from %s: %s\n", src_path, strerror(errno));
				exit(1);
			}
			if(write(tgtfd, buffer, size_read) == -1) {
				fprintf(stderr, "Unable to write to %s: %s\n", tgt_path, strerror(errno));
				exit(1);
			}
		}
		close(srcfd);
		close(tgtfd);
	}
	uint64_t end_time = get_time();
	printf("%" PRIu64 "\n", end_time - start_time);
}
