#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <syscall.h>

#include "defuse.h"

#define SEEK_DATA 3
#define SEEK_HOLE 4

off_t real_lseek(int fd, off_t offset, int whence)
{
#ifdef SYS__llseek
	off_t result;
	return syscall(SYS__llseek, fd, offset>>32, offset, &result, whence) ? -1 : result;
#else
	return syscall(SYS_lseek, fd, offset, whence);
#endif
}

int defuse_lseek(const struct file_handle_data* fhd, int fd, off_t offset, int whence)
{
	DEBUG_ENTER;
	int rv = 0;
	if(whence == SEEK_CUR || whence == SEEK_SET) {
		rv = real_lseek(fd, offset, whence);
		goto out;
	}

	struct stat statbuf;
	rv = fhd->mount->fgetattr(fhd->file_handle, &statbuf);
	if(rv != 0) {
		errno = rv;
		rv = -1;
		goto out;
	}
	switch(whence) {
		case SEEK_HOLE:
			if(offset >= statbuf.st_size) {
				errno = ENXIO;
				rv = -1;
				goto out;
			}
			offset = statbuf.st_size;
			break;
		case SEEK_DATA:
			if(offset >= statbuf.st_size) {
				errno = ENXIO;
				rv = -1;
				goto out;
			}
			break;
		case SEEK_END:
			offset += statbuf.st_size;
			break;
	}
	rv = real_lseek(fd, offset, SEEK_SET);
out:
	DEBUG_EXIT(rv);
	return rv;
}

#undef SEEK_DATA
#undef SEEK_HOLE
