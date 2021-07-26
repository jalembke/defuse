#include <sys/types.h>
#include "libdefuse.h"
#include "glibc_ops.h"

int fsync(int fd) 
{
	DEFUSE_FD_OP(fsync, fd, fd, 0);
	return real_ops.fsync(fd);
}
