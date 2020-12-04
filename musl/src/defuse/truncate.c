#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "defuse.h"

int defuse_truncate(const struct mount_point_data* mp, const char* cpath, const char* pathname, off_t length)
{
	DEBUG_ENTER;
	int rv = mp->truncate(cpath, length);
	if (rv != 0) {
		errno = rv;
		rv = -1;
	}
	DEBUG_EXIT(rv);
	return rv;
}

int defuse_ftruncate(const struct file_handle_data* fhd, int fd, off_t length)
{
	DEBUG_ENTER;
	int rv = fhd->mount->ftruncate(fhd->file_handle, length);
	if(rv != 0) {
		errno = rv;
		rv = -1;
	}
	DEBUG_EXIT(rv);
	return rv;
}
