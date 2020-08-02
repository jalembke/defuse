#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "defuse.h"

int defuse_fsync(const struct file_handle_data* fhd, int fd, int data_sync)
{
	DEBUG_ENTER;
	int rv = fhd->mount->fsync(fhd->file_handle, data_sync);
	if(rv != 0) {
		errno = rv;
		rv = -1;
	}
	DEBUG_EXIT(rv);
	return rv;
}
