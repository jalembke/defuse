#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "libdefuse.h"

ssize_t defuse_read(const struct file_handle_data* fhd, int fd, void* buf, size_t count)
{
	DEBUG_ENTER;
	ssize_t rv = 0;

	struct flock file_lock;
	file_lock.l_type = F_RDLCK;
	file_lock.l_whence = SEEK_SET;
	file_lock.l_start = 0;
	file_lock.l_len = 1;

	int fcntl_rv = fcntl(fd, F_SETLK, &file_lock);
	if (fcntl_rv == -1) {
		goto out;
	}

	file_lock.l_type = F_UNLCK;
	off_t offset = real_lseek(fd, 0, SEEK_CUR);
	if (offset != (off_t)-1) {
		size_t bytes_read = 0;
		int ret_rc = fhd->mount->read(fhd->file_handle, (char*)buf, count, offset, &bytes_read);
		if(ret_rc != 0) {
			errno = ret_rc;
			rv = -1;
		} else {
			real_lseek(fd, offset + bytes_read, SEEK_SET);
			fcntl(fd, F_SETLK, &file_lock);
			errno = 0;
			rv = bytes_read;
		}
	}
out:
	DEBUG_EXIT(rv);
	return rv;
}
