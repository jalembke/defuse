#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

static inline char* get_path(char* path, size_t size)
{
	if(-1 == readlink("/proc/self/exe", path, size)) {
		fprintf(stderr, "Failed to read /proc/self/exe: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	char* last_slash = strrchr(path, '/');
	if(last_slash) {
		(*last_slash) = 0;
	}
	return path;
}

#define NSEC_PER_SEC 1000000000
static inline uint64_t get_time() 
{
	struct timespec ts;
	if(clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		fprintf(stderr, "Unable to retrieve current time: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return ts.tv_sec * NSEC_PER_SEC + ts.tv_nsec;
}
#undef NSEC_PER_SEC

#endif // _UTIL_H
