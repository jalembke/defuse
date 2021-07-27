#include <errno.h>

#include "libdefuse.h"

int defuse_unlink(const struct mount_point_data* mp, const char* cpath, const char* filename)
{
	DEBUG_ENTER;
	int rv = mp->unlink(cpath);
	if (rv != 0) {
		errno = rv;
		rv = -1;
	}
	DEBUG_EXIT(rv);
	return rv;
}
