#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "defuse.h"

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
