#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <string>

#include "shared_space.h"

static inline size_t get_file_size(int fd)
{
	off_t current_pos = syscall(SYS_lseek, fd, 0, SEEK_SET);
	off_t size = syscall(SYS_lseek, fd, 0, SEEK_END);
	syscall(SYS_lseek, fd, current_pos, SEEK_SET);
	return size;
}

static inline std::string get_shared_space_file_name()
{
	return std::string("/tmp/DEFUSE_") + std::to_string(getpid());
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
	shared_space& space = shared_space::getInstance();
	if(space.xFd == -1) {
		space.xFd = syscall(SYS_open, get_shared_space_file_name(), O_RDWR);
		assert(space.xFd != -1);
		space.xSize = get_file_size(space.xFd);
		space.xPtr = mmap(NULL, space.xSize, PROT_READ | PROT_WRITE, MAP_SHARED, space.xFd, 0);
		assert(space.xPtr != MAP_FAILED);
	}
	return space.xPtr;
}

void* shared_space::init(size_t size)
{
	shared_space& space = shared_space::getInstance();
	if(space.xFd != -1) {
		munmap(space.xPtr, space.xSize);
	}
	space.xFd = syscall(SYS_open, get_shared_space_file_name(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	assert(space.xFd != -1);
	
	space.xSize = size;
	syscall(SYS_ftruncate, size);
	
	space.xPtr = mmap(NULL, space.xSize, PROT_READ | PROT_WRITE, MAP_SHARED, space.xFd, 0);
	assert(space.xPtr != MAP_FAILED);
	return space.xPtr;
}
