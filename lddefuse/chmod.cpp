#include <errno.h>
#include <unistd.h>

#include "lddefuse.h"
#include "glibc_ops.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

int chmod(const char *path, mode_t mode) 
{   
	int ret = 0;

	OP_ENTER;
	ret = fs->chmod(cpath.c_str(), mode);
	if(ret != 0) {
		errno = ret;
		ret = -1;
	}
	OP_EXIT(chmod, (path, mode));
    
    return ret;
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
