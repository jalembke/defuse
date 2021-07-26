#include "libdefuse.h"
#include "glibc_ops.h"

ssize_t write(int fd, const void *buf, size_t count)
{
	DEFUSE_FD_OP(write, fd, fd, buf, count);
	return real_ops.write(fd, buf, count);
}
