#include "libdefuse.h"
#include "glibc_ops.h"

int close(int fd) 
{
	DEFUSE_FD_OP(close, fd, fd);
	return real_ops.close(fd);
}
