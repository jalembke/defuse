#include <errno.h>
#include <unistd.h>
#include <syscall.h>

#include "defuse.h"

int real_close(int fd)
{
	return syscall(SYS_close, fd);
}

int defuse_close(const struct file_handle_data* fhd, int fd)
{
	DEBUG_ENTER;
	int rv = fhd->mount->close(fhd->file_handle);
	if(rv != 0) {
		errno = rv;
		rv = -1;
	} else {
		rv = remove_file_handle(fd);
		if (rv == 0) {
			rv = real_close(fd);
		} else {
			errno = rv;
			rv = -1;
		}
	}
	DEBUG_EXIT(rv);
	return rv;
}
