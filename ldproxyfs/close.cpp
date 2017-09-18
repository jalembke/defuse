#include <errno.h>
#include <unistd.h>

#include "ldhykefs.h"
#include "glibc_ops.h"
#include "file_handle_data.h"
#include "dir_handle_data.h"
#include "HybridFS.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

int close(int fd) 
{	
	int ret = 0;
    int fsret = remove_file_handle(fd);
	if(fsret != ENOENT) {
		DEBUG_ENTER;
		DEBUG_EXIT(fsret);
	}
	ret = real_ops.close(fd);

	if(fsret != ENOENT && fsret != 0) {
		ret = -1;
		errno = fsret;
	}

    return ret;
}

int closedir(DIR *dirp)
{
	int ret = 0;

	DIR_OP_ENTER;
	remove_dir_handle(dirp);
	OP_EXIT(closedir, (dirp));

	return ret;
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
