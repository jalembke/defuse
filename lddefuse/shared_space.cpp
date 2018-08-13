#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <string>

#include "debug.h"
#include "shared_space.h"

static inline size_t get_file_size(int fd)
{
	off_t current_pos = syscall(SYS_lseek, fd, 0, SEEK_SET);
	off_t size = syscall(SYS_lseek, fd, 0, SEEK_END);
	syscall(SYS_lseek, fd, current_pos, SEEK_SET);
	return size;
}

static inline char* get_shared_space_file_name()
{
	static char space_path_str[32];
	snprintf(space_path_str, sizeof(space_path_str), "/tmp/DEFUSE_%d", getpid());
	return space_path_str;
}

shared_space::~shared_space()
{
	if(xFd != -1) {
		munmap(xPtr, xSize);
		syscall(SYS_close, xFd);
		xFd = -1;
	}
}

shared_space& shared_space::getInstance()
{
	static shared_space space;
	return space;
}

void* shared_space::get()
{
	DEBUG_ENTER;
	shared_space& space = shared_space::getInstance();
	if(space.xFd == -1) {
		space.xFd = syscall(SYS_open, get_shared_space_file_name(), O_RDWR);
		if(space.xFd == -1) {
			return NULL;
		}
		space.xSize = get_file_size(space.xFd);
		space.xPtr = mmap(NULL, space.xSize, PROT_READ | PROT_WRITE, MAP_SHARED, space.xFd, 0);
		assert(space.xPtr != MAP_FAILED);
	}
	DEBUG_EXIT(space.xPtr);
	return space.xPtr;
}

void* shared_space::init(size_t size)
{
	DEBUG_ENTER;
	shared_space& space = shared_space::getInstance();
	if(space.xFd != -1) {
		munmap(space.xPtr, space.xSize);
	}
	space.xFd = syscall(SYS_open, get_shared_space_file_name(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	assert(space.xFd != -1);
	DEBUG_PRINT(space.xFd);

	space.xSize = size;
	assert(syscall(SYS_ftruncate, space.xFd, size) != -1);
	DEBUG_PRINT(space.xSize);
	
	space.xPtr = mmap(NULL, space.xSize, PROT_READ | PROT_WRITE, MAP_SHARED, space.xFd, 0);
	assert(space.xPtr != MAP_FAILED);

	DEBUG_EXIT(space.xPtr);
	return space.xPtr;
}
