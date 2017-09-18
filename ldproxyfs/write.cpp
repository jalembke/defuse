#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#include "ldproxyfs.h"
#include "glibc_ops.h"
#include "file_handle_data.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

ssize_t write(int fd, const void *buf, size_t count)
{
    ssize_t ret = -1;

	FD_OP_ENTER;
        
	off_t offset = lseek(fd, 0, SEEK_CUR);
	if(offset != (off_t)-1) {
		size_t bytes_written = 0;

		DEBUG_PRINT(fhd->file_handle);

		int ret_rc = fhd->file_system->write(fhd->file_handle, (const char *)buf, count, offset, &bytes_written);
		if(ret_rc != 0) {
			errno = ret_rc;
			ret = -1;
		} else {
			ret = (ssize_t)bytes_written;
			lseek(fd, offset + ret, SEEK_SET);
		}
	}

	OP_EXIT(write, (fd, buf, count));

    return ret;
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
