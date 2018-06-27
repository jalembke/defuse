#include <errno.h>
#include <unistd.h>

#include "lddefuse.h"
#include "glibc_ops.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

int access(const char *path, int mode)
{   
	int ret = 0;

	OP_ENTER;        
	ret = fs->access(cpath.c_str(), mode);
  	if(ret != 0) {
		errno = ret;
		ret = -1;
	}
	OP_EXIT(access, (path, mode));
    
    return ret;
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
