#include "libdefuse.h"
#include "glibc_ops.h"

int access(const char *path, int mode) 
{   
	DEFUSE_OP(access, path, path, mode);
	return real_ops.access(path, mode);
}
