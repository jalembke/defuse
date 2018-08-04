#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <wait.h>

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

std::string get_path()
{
	char buffer[PATH_MAX + 1] = {0};
	if(-1 == readlink("/proc/self/exe", buffer, sizeof(buffer))) {
		fprintf(stderr, "Failed to read /proc/self/exe: %s\n", strerror(errno));
		exit(1);
	}
	std::string exepath(buffer);
	return exepath.substr(0, exepath.find_last_of("\\/"));
}

int main(int argc, char* argv[])
{
	if(argc != 4 && argc != 5) {
		printf("Usage: doCopy srcdir tgtdir count\n");
		exit(1);
	}

	std::string src = argv[1];
	src += "/0";
	std::string tgt = argv[2];
	tgt += "/0";
	int file_count = atoi(argv[3]);

	bool with_fork = false;
	pid_t child_pid = 0;
	if (argc == 5) {
		with_fork = true;
	}

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

		if(with_fork) {
			std::string exepath = get_path();
			exepath.append("/doCat");
			child_pid = fork();
			if(-1 == child_pid) {
				fprintf(stderr, "Unable for fork: %s\n", strerror(errno));
				exit(1);
			}
			if(child_pid == 0) {
				dup2(srcfd, STDIN_FILENO);
				dup2(tgtfd, STDOUT_FILENO);
				char* const parm_list[] = {(char*)exepath.c_str(), NULL};
				if(-1 == execv(exepath.c_str(), parm_list)) {
					fprintf(stderr, "Unable to exec %s in child: %s\n", exepath.c_str(), strerror(errno));
					exit(1);
				}
			} else {
				waitpid(child_pid, NULL, WNOHANG);
			}
		} else {
			int size_read = read(srcfd, buffer, sizeof(buffer));
			if(size_read == -1) {
				fprintf(stderr, "Unable to read from %s: %s\n", (src + std::to_string(i)).c_str(), strerror(errno));
				exit(1);
			}
			if(write(tgtfd, buffer, size_read) == -1) {
				fprintf(stderr, "Unable to write to %s: %s\n", (tgt + std::to_string(i)).c_str(), strerror(errno));
				exit(1);
			}
		}
		close(srcfd);
		close(tgtfd);
	}
	uint64_t end_time = getTime();
	printf("%" PRIu64 "\n", end_time - start_time);
}
