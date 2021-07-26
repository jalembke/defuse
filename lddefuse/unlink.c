#include "libdefuse.h"
#include "glibc_ops.h"

int unlink(const char *path) 
{   
	DEFUSE_OP(unlink, path, path);
	return real_ops.unlink(path);
}
