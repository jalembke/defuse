#include <errno.h>

#include "libdefuse.h"

ssize_t defuse_readlink(const struct mount_point_data* mp, const char* cpath, const char* pathname, char* buf, size_t bufsiz)
{
	DEBUG_ENTER;
	ssize_t rv = 0;

	size_t bytes_written = 0;
	int ret_rc = mp->readlink(cpath, buf, bufsiz, &bytes_written);
	if(ret_rc != 0) {
		errno = ret_rc;
		rv = -1;
	} else {
		errno = 0;
		rv = bytes_written;
	}
	DEBUG_EXIT(rv);
	return rv;
}
