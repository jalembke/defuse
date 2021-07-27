#include <errno.h>

#include "libdefuse.h"

int defuse_access(const struct mount_point_data* mp, const char* cpath, const char* filename, int mode)
{
	DEBUG_ENTER;
	int rv = mp->access(cpath, mode);
	if (rv != 0) {
		errno = rv;
		rv = -1;
	}
	DEBUG_EXIT(rv);
	return rv;
}
