#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "libdefuse.h"

struct shared_space {
	int fd;
	size_t size;
	void* space_ptr;
};

static inline size_t get_file_size(int fd)
{
	size_t size = 0;
	size_t current_pos = 0;

	struct flock file_lock;
	file_lock.l_type = F_RDLCK;
	file_lock.l_whence = SEEK_SET;
	file_lock.l_start = 0;
	file_lock.l_len = 1;
	int fcntl_rv = fcntl(fd, F_SETLK, &file_lock);
	if (fcntl_rv == -1) {
		goto out;
	}
	file_lock.l_type = F_UNLCK;

	current_pos = real_lseek(fd, 0, SEEK_SET);
	size = real_lseek(fd, 0, SEEK_END);
	real_lseek(fd, current_pos, SEEK_SET);
	fcntl(fd, F_SETLK, &file_lock);
out:
	return size;
}

static inline char* get_shared_space_file_name()
{
	static char space_path_str[32];
	snprintf(space_path_str, sizeof(space_path_str), "/tmp/defuse_%d", getpid());
	return strdup(space_path_str);
}

static inline struct shared_space* get_instance()
{
	static struct shared_space ss_instance;
	static bool ss_initialized = false;
	if(ss_initialized == false) {
		ss_instance.fd = -1;
		ss_instance.size = 0;
		ss_instance.space_ptr = NULL;
		ss_initialized = true;
	}
	return &ss_instance;
}

void release_shared_space()
{
	struct shared_space* space = get_instance();
	if(space->fd >= 0) {
		munmap(space->space_ptr, space->size);
		real_close(space->fd);
		space->fd = -1;
	}
}

void* get_shared_space()
{
	DEBUG_ENTER;
	void* rv = NULL;
	struct shared_space* space = get_instance();
	if(space->fd == -1) {
		char* shared_space_file = get_shared_space_file_name();
		DEBUG_PRINT(shared_space_file);
		space->fd = real_open(shared_space_file, O_RDWR, 0);
		free(shared_space_file);
		if(space->fd == -1) {
			goto out;
		}
		space->size = get_file_size(space->fd);
		space->space_ptr = real_mmap(NULL, space->size, PROT_READ | PROT_WRITE, MAP_SHARED, space->fd, 0);
		if(space->space_ptr != MAP_FAILED) {
			rv = space->space_ptr;
		}
	} else {
		rv = space->space_ptr;
	}
out:
	DEBUG_EXIT(rv);
	return rv;
}

void* init_shared_space(size_t size)
{
	DEBUG_ENTER;
	void* rv = NULL;
	struct shared_space* space = get_instance();
	if(space->fd != -1) {
		munmap(space->space_ptr, space->size);
	}
	char* shared_space_file = get_shared_space_file_name();
	space->fd = real_open(shared_space_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	free(shared_space_file);
	if(space->fd == -1) {
		goto out;
	}
	space->size = size;
	if(real_ftruncate(space->fd, size) == -1) {
		goto out;
	}
	space->space_ptr = real_mmap(NULL, space->size, PROT_READ | PROT_WRITE, MAP_SHARED, space->fd, 0);
	if(space->space_ptr != MAP_FAILED) {
		rv = space->space_ptr;
	}
out:
	DEBUG_EXIT(rv);
	return rv;
}
