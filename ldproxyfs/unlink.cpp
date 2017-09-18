#include <errno.h>
#include <unistd.h>

#include "ldproxyfs.h"
#include "glibc_ops.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

int unlink(const char *path) 
{   
	int ret = 0;

	OP_ENTER;
	ret = fs->unlink(cpath.c_str());
	if(ret != 0) {
		errno = ret;
		ret = -1;
	}
	OP_EXIT(unlink, (path));
    
    return ret;
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
