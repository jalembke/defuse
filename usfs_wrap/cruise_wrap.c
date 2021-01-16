#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>

#include "cruise.h"
#include "cruise-internal.h"

static char* backend = NULL;

void usfs_init(const char* mount_path, const char* backend_path)
{
	cruise_mount(backend_path, 1024 * 1024, 0);
	backend = strdup(backend_path);
}

int usfs_open(const char* path, int flags, mode_t mode, uint64_t* ret_fh)
{
	int ret = 0;
	char path_to_open[PATH_MAX];
	snprintf(path_to_open, PATH_MAX, "%s/%s", backend, path);
	//printf("OPEN: %s %o %o\n", path_to_open, flags, mode);
	int fd = CRUISE_WRAP(open)(path_to_open, flags, mode);
	if (fd >= 0) {
		*ret_fh = (uint64_t)fd;
	} else {
		ret = errno;
	}
	return ret;
}

int usfs_read(uint64_t fh, char *buf, size_t size, off_t offset, size_t* bytes_read)
{
	int ret = CRUISE_WRAP(pread)((int)fh, buf, size, offset);
	if(-1 == ret) {
		ret = errno;
		*bytes_read = 0;
	} else {
		*bytes_read = ret;
		ret = 0;
	}
	return ret;
}

int usfs_write(uint64_t fh, const char *buf, size_t size, off_t offset, size_t* bytes_written)
{
	int ret = CRUISE_WRAP(pwrite)((int)fh, buf, size, offset);
	if(-1 == ret) {
		ret = errno;
		*bytes_written = 0;
	} else {
		*bytes_written = ret;
		ret = 0;
	}
	return ret;
}

int usfs_fsync(uint64_t fh, int data_sync)
{
	int ret = 0;
	if(!data_sync) {
		ret = CRUISE_WRAP(fsync)((int)fh);
    } else {
		ret = CRUISE_WRAP(fdatasync)((int)fh);
	}
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_truncate(const char* path, off_t length)
{
	int ret = 0;
	char path_to_truncate[PATH_MAX];
	snprintf(path_to_truncate, PATH_MAX, "%s/%s", backend, path);
	ret = CRUISE_WRAP(truncate)(path_to_truncate, length);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_ftruncate(uint64_t fh, off_t offset)
{
	int ret = CRUISE_WRAP(ftruncate)((int)fh, offset);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_fgetattr(uint64_t fh, struct stat *stbuf)
{
	int ret = CRUISE_WRAP(stat)((int)fh, stbuf);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_getattr(const char* path, struct stat *stbuf, int flags)
{
	int ret = 0;
	char path_to_stat[PATH_MAX];
	snprintf(path_to_stat, PATH_MAX, "%s/%s", backend, path);
	ret = CRUISE_WRAP(stat)(path_to_stat, stbuf);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_unlink(const char* path)
{
	int ret =0;
	char path_to_unlink[PATH_MAX];
	snprintf(path_to_unlink, PATH_MAX, "%s/%s", backend, path);
	ret = CRUISE_WRAP(unlink)(path_to_unlink);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_close(uint64_t fh)
{
	return CRUISE_WRAP(close)((int)fh);
}
