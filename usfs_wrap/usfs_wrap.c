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

static char* backend = NULL;

void usfs_init (const char* mount_path, const char* backend_path)
{
	printf("INIT: %s %s\n", mount_path, backend_path);
	backend = strdup(backend_path);
}

int usfs_open(const char* path, int flags, mode_t mode, uint64_t* ret_fh)
{
	char path_to_open[PATH_MAX];
	snprintf(path_to_open, PATH_MAX, "%s/%s", backend, path);
	printf("OPEN: %s\n", path_to_open);
	int fd = syscall(SYS_open, path_to_open, flags, mode);
	if (fd > 0) {
		*ret_fh = (uint64_t)fd;
	}
	*ret_fh = 0;
	return 0;
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

int usfs_close(uint64_t ret_fh)
{
	return syscall(SYS_close, (int)ret_fh);
}
