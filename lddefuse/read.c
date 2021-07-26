#include "libdefuse.h"
#include "glibc_ops.h"

ssize_t read(int fd, void *buf, size_t count) 
{
	DEFUSE_FD_OP(read, fd, fd, buf, count);
	return real_ops.read(fd, buf, count);
}
