#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/syscall.h>
#include <inttypes.h>

static char* backend = NULL;

void usfs_init(const char* mount_path, const char* backend_path)
{
	backend = strdup(backend_path);
}

int usfs_open(const char* path, int flags, mode_t mode, uint64_t* ret_fh)
{
	int ret = 0;
	char path_to_open[PATH_MAX];
	snprintf(path_to_open, PATH_MAX, "%s/%s", backend, path);
	int fd = syscall(SYS_open, path_to_open, flags, mode);
	if (fd > 0) {
		*ret_fh = (uint64_t)fd;
	} else {
		ret = errno;
	}
	return ret;
}

int usfs_read(uint64_t fh, char *buf, size_t size, off_t offset, size_t* bytes_read)
{
	int ret = syscall(SYS_pread64, fh, buf, size, offset);
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
	int ret = syscall(SYS_pwrite64, fh, buf, size, offset);
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
	if(data_sync > 0) {
		ret = syscall(SYS_fdatasync, fh);
	} else {
		ret = syscall(SYS_fsync, fh);
	}
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_truncate(const char* path, off_t length)
{
	int ret =0;
	char path_to_truncate[PATH_MAX];
	snprintf(path_to_truncate, PATH_MAX, "%s/%s", backend, path);
	ret = syscall(SYS_truncate, path_to_truncate, length);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_ftruncate(uint64_t fh, off_t offset)
{
	int ret = syscall(SYS_ftruncate, fh, offset);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_fgetattr(uint64_t fh, struct stat *stbuf)
{
	int ret = syscall(SYS_fstat, (int)fh, stbuf);
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
	if (flags & AT_SYMLINK_NOFOLLOW) {
		ret = syscall(SYS_lstat, path_to_stat, stbuf);
	} else {
		ret = syscall(SYS_stat, path_to_stat, stbuf);
	}
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_unlink(const char* path)
{
	int ret = 0;
	char path_to_unlink[PATH_MAX];
	snprintf(path_to_unlink, PATH_MAX, "%s/%s", backend, path);
	ret = syscall(SYS_unlink, path_to_unlink);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_access(const char* path, int mode)
{
	int ret = 0;
	char path_to_access[PATH_MAX];
	snprintf(path_to_access, PATH_MAX, "%s/%s", backend, path);
	ret = syscall(SYS_access, path_to_access, mode);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int usfs_close(uint64_t ret_fh)
{
	return syscall(SYS_close, (int)ret_fh);
}
