#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "defuse.h"

int defuse_open(const struct mount_point_data* mp, const char* cpath, const char* filename, int flags, mode_t mode)
{
	DEBUG_ENTER;
	int rv = 0;
	uint64_t fh = 0;

	rv = mp->open(cpath, flags, mode, &fh);
	DEBUG_PRINT_INT(rv);
	if (rv == 0) {
		struct file_handle_data fhd;
		fhd.file_handle = fh;
		fhd.mount = mp;
		int real_fd = real_open(filename, O_RDONLY, 0);
		if(real_fd >= 0) {
			rv = insert_file_handle(real_fd, &fhd);
			if (rv != 0) {
				mp->close(fh);
				errno = rv;
				rv = -1;
			} else {
				rv = real_fd;
			}
		} else {
			mp->close(fh);
			rv = real_fd;
		}
	} else {
		errno = rv;
		rv = -1;
	}
	DEBUG_EXIT(rv);
	return rv;
}
