#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#include "lddefuse.h"
#include "glibc_ops.h"
#include "file_handle_data.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

int fsync(int fd) 
{
    int ret = -1;

	FD_OP_ENTER;

	int ret_rc = fhd->file_system->fsync(fhd->file_handle);
	if(ret_rc != 0) {
		errno = ret_rc;
		ret = -1;
	} else {
		ret = 0;
	}

	OP_EXIT(fsync, (fd));

    return ret;
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
