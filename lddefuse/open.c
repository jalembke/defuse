#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>

#include "libdefuse.h"
#include "glibc_ops.h"

static inline int open_common(const char *path, int flags, mode_t mode) 
{
	DEFUSE_OP(open, path, path, flags, mode);
	return real_ops.open(path, flags, mode);
}

int openat(int dirfd, const char* path, int flags, ...)
{
	mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list argf;
        va_start(argf, flags);
        mode = va_arg(argf, mode_t);
        va_end(argf);
    }
	return open_common(path, flags, mode);
}

int open(const char *path, int flags, ...) 
{	
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list argf;
        va_start(argf, flags);
        mode = va_arg(argf, mode_t);
        va_end(argf);
    }
	return open_common(path, flags, mode);
}

int creat(const char* path, mode_t mode)
{
	return open_common(path, O_CREAT|O_WRONLY|O_TRUNC, mode);
}

int open64(const char *path, int flags, ...) 
{
#ifdef _LARGEFILE64_SOURCE
	mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list argf;
        va_start(argf, flags);
        mode = va_arg(argf, mode_t);
        va_end(argf);
    }

	return open_common(path, flags | O_LARGEFILE, mode);
#else
	return ENOSYS;
#endif
}

int creat64(const char* path, mode_t mode)
{
#ifdef _LARGEFILE64_SOURCE
	return open_common(path, O_CREAT|O_WRONLY|O_TRUNC|O_LARGEFILE, mode);
#else
	return ENOSYS;
#endif
}
