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

#include <virtual.h>

static char* backend = NULL;

static pthread_mutex_t avfs_mutexlock = PTHREAD_MUTEX_INITIALIZER;

void usfs_init (const char* mount_path, const char* backend_path)
{
	//printf("INIT: %s %s\n", mount_path, backend_path);
	backend = strdup(backend_path);
}

int usfs_open(const char* path, int flags, mode_t mode, uint64_t* ret_fh)
{
	int ret = 0;
	char path_to_open[PATH_MAX];
	snprintf(path_to_open, PATH_MAX, "%s/%s", backend, path);
	//printf("OPEN: %s %o %o\n", path_to_open, flags, mode);
	int fd = virt_open(path_to_open, flags, mode);
	if (fd >= 0) {
		*ret_fh = (uint64_t)fd;
	} else {
		ret = errno;
	}
	return ret;
}

int usfs_read(uint64_t fh, char *buf, size_t size, off_t offset, size_t* bytes_read)
{
	int ret = 0;
	
	pthread_mutex_lock(&avfs_mutexlock);
	if (virt_lseek((int)fh, offset, SEEK_SET) == -1) {
		ret = errno;
		*bytes_read = 0;
	} else {
		ret = virt_read((int)fh, buf, size);
		if(-1 == ret) {
			ret = errno;
			*bytes_read = 0;
		} else {
			*bytes_read = ret;
			ret = 0;
		}
	}
	pthread_mutex_unlock(&avfs_mutexlock);
	return ret;
}

int usfs_write(uint64_t fh, const char *buf, size_t size, off_t offset, size_t* bytes_written)
{
	int ret = 0;
	//printf("WRITE: %" PRIu64 " %zd %zd\n", fh, size, offset);
	pthread_mutex_lock(&avfs_mutexlock);
	if (virt_lseek((int)fh, offset, SEEK_SET) == -1) {
		ret = errno;
		*bytes_written = 0;
	} else {
		ret = virt_write((int)fh, buf, size);
		if(-1 == ret) {
			ret = errno;
			*bytes_written = 0;
		} else {
			*bytes_written = ret;
			ret = 0;
		}
	}
	pthread_mutex_unlock(&avfs_mutexlock);
	return ret;
}

int usfs_fsync(uint64_t fh, int data_sync)
{
	return 0;
}

int usfs_truncate(const char* path, off_t length)
{
	int ret =0;
	char path_to_truncate[PATH_MAX];
	snprintf(path_to_truncate, PATH_MAX, "%s/%s", backend, path);
	ret = virt_truncate(path_to_truncate, length);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_ftruncate(uint64_t fh, off_t offset)
{
	int ret = virt_ftruncate((int)fh, offset);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_fgetattr(uint64_t fh, struct stat *stbuf)
{
	int ret = virt_fstat((int)fh, stbuf);
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
	//printf("GETATTR %s %o\n", path_to_stat, flags);
	if (flags & AT_SYMLINK_NOFOLLOW) {
		ret = virt_lstat(path_to_stat, stbuf);
	} else {
		ret = virt_stat(path_to_stat, stbuf);
	}
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
	ret = virt_unlink(path_to_unlink);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_close(uint64_t fh)
{
	return virt_close(fh);
}
