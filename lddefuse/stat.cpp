#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "lddefuse.h"
#include "glibc_ops.h"
#include "file_handle_data.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

#define STAT_WRAPPER(func, stype, flags) \
int func(int vers, const char* path, struct stype *buf) \
{ \
	int ret = 0; \
\
	OP_ENTER; \
	ret = fs->getattr(cpath.c_str(), (struct stat*)buf, flags); \
	if(ret != 0) { \
		errno = ret; \
		ret = -1; \
	} \
	OP_EXIT(func, (vers, path, buf)); \
\
	return ret; \
}

#define FSTAT_WRAPPER(func, stype) \
int func(int vers, int fd, struct stype *buf) \
{ \
	int ret = 0; \
\
	FD_OP_ENTER; \
	ret = fhd->file_system->fgetattr(fhd->file_handle, (struct stat*)buf); \
	if(ret != 0) { \
		errno = ret; \
		ret = -1; \
	} \
	OP_EXIT(func, (vers, fd, buf)); \
\
	return ret; \
}

#define FSTATAT_WRAPPER(func, stype) \
int func(int vers, int dirfd, const char* path, struct stype *buf, int flags) \
{ \
	int ret = 0; \
\
	AT_OP_ENTER; \
	ret = fs->getattr(cpath.c_str(), (struct stat*)buf, flags); \
	if(ret != 0) { \
		errno = ret; \
		ret = -1; \
	} \
	OP_EXIT(func, (vers, dirfd, path, buf, flags)); \
\
	return ret; \
}

STAT_WRAPPER(__xstat, stat, 0);
STAT_WRAPPER(__xstat64, stat64, 0);
STAT_WRAPPER(__lxstat, stat, AT_SYMLINK_NOFOLLOW);
STAT_WRAPPER(__lxstat64, stat64, AT_SYMLINK_NOFOLLOW);
FSTAT_WRAPPER(__fxstat, stat);
FSTAT_WRAPPER(__fxstat64, stat64);
FSTATAT_WRAPPER(__fxstatat, stat);
FSTATAT_WRAPPER(__fxstatat64, stat64);

#define XATTR_WRAPPER(func, args, parms) \
ssize_t func args \
{ \
	ssize_t ret = 0; \
\
	OP_ENTER; \
	errno = ENOTSUP; \
	ret = -1; \
   	OP_EXIT(func, parms); \
\
    return ret; \
}

#define XATTR_WRAPPER_FD(func, args, parms) \
ssize_t func args { \
\
	ssize_t ret = 0; \
\
	FD_OP_ENTER; \
	errno = ENOTSUP; \
	ret = -1; \
    OP_EXIT(func, parms);\
\
    return ret; \
}

XATTR_WRAPPER(getxattr, (const char *path, const char *name, void *value, size_t size), (path, name, value, size));
XATTR_WRAPPER(lgetxattr, (const char *path, const char *name, void *value, size_t size), (path, name, value, size));
XATTR_WRAPPER_FD(fgetxattr, (int fd, const char *name, void *value, size_t size), (fd, name, value, size));

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
