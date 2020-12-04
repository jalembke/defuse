#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "defuse.h"

int defuse_getattr(const struct mount_point_data* mp, const char* cpath, const char* pathname, struct stat* statbuf, int flags)
{
	DEBUG_ENTER;
	int rv = mp->getattr(cpath, statbuf, flags);
	if (rv != 0) {
		errno = rv;
		rv = -1;
	}
	DEBUG_EXIT(rv);
	return rv;
}

int defuse_getattrat(const struct mount_point_data* mp, const char* cpath, int dirfd, const char* pathname, struct stat* statbuf, int flags)
{
	return defuse_getattr(mp, cpath, pathname, statbuf, flags);
}

int defuse_fgetattr(const struct file_handle_data* fhd, int fd, struct stat* statbuf)
{
	DEBUG_ENTER;
	int rv = fhd->mount->fgetattr(fhd->file_handle, statbuf);
	if(rv != 0) {
		errno = rv;
		rv = -1;
	}
	DEBUG_EXIT(rv);
	return rv;
}
