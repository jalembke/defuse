#ifndef SSHFS_OP_H
#define SSHFS_OP_H

#include <sys/types.h>
#include <stdint.h>

struct sshfs_operations {
	int (*read)(char *rbuf, size_t size, off_t offset, uint64_t *fi),
	int (*write)(const char *wbuf, size_t size, off_t offset, uint64_t *fi),
	int (*fsync)(int isdatasync, uint64_t *fi),
	int (*release)(const char *path, uint64_t *fi),
	int (*truncate)(const char *path, off_t size, uint64_t *fi),
	int (*getattr)(const char *path, struct stat *stbuf, uint64_t *fi),
	int (*unlink)(const char *path);
};

#endif
