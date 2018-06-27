#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "lddefuse.h"
#include "glibc_ops.h"
#include "file_handle_data.h"
#include "dir_handle_data.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

static inline file_handle_data_ptr open_internal(const std::string& cpath, const char* path, FileSystemWrapper* fs, int flags, int mode)
{
	int ret;
	uint64_t fh = 0;
	file_handle_data_ptr fhd;

	static int real_fd_counter = 10;

	DEBUG_PRINT(cpath << " " << path << " " << flags);
	ret = fs->open(cpath.c_str(), flags, mode, &fh);
	if(ret == 0) {

		DEBUG_PRINT(cpath << " " << path << " " << (flags & (~O_CREAT) & (~O_EXCL) & (~O_TRUNC)));
		// Open the file au naturel
		//   If the file was to be created or truncated, it would have already
		//     been done by the file system open
#if defined(DEFUSE_DO_REAL_OPEN)
		int real_fd = syscall(SYS_open, path, flags & (~O_CREAT) & (~O_EXCL) & (~O_TRUNC), mode);
#elif defined(DEFUSE_DO_NULL_OPEN)
		int real_fd = (flags & O_DIRECTORY) ?
			syscall(SYS_open, "/", flags & (~O_CREAT) & (~O_TRUNC), mode) :
			syscall(SYS_open, "/dev/null", flags & (~O_CREAT) & (~O_TRUNC), mode);
#elif defined(DEFUSE_DO_NO_OPEN)
		int real_fd = real_fd_counter++;
#endif
		// Store the file info in the map
		if(real_fd != -1) {
			DEBUG_PRINT(real_fd << " " << fh);
			fhd = file_handle_data_ptr(new file_handle_data(fs, fh, real_fd));
			insert_file_handle(real_fd, fhd);
		} else {
			DEBUG_PRINT("ERROR 2 " << errno);
			ret = errno;
			fs->close(fh);
		}
		
	} else {
		DEBUG_PRINT("ERROR 1 " << errno);
		ret = errno;
	}

	return fhd;
}

static inline int open_common(const char *path, int flags, mode_t mode) 
{
	int ret = -1;

	OP_ENTER;

	file_handle_data_ptr fhd = open_internal(cpath, path, fs, flags, mode);
	if(fhd) {
		ret = fhd->file_descriptor;

		DEBUG_PRINT(fhd->file_handle << " " << fhd->file_descriptor);
	}

	OP_EXIT(open, (path, flags, mode));

	return ret;
}

static inline mode_t get_umask()
{
	mode_t current_umask = umask(0);
	umask(current_umask);
	return current_umask;
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

DIR* opendir(const char *path)
{
	DIR* ret = NULL;

	OP_ENTER;

	file_handle_data_ptr fhd = open_internal(cpath, path, fs, O_RDONLY | O_DIRECTORY | O_CLOEXEC, 0);
	if(fhd) {
		dir_handle_data_ptr dhd = dir_handle_data_ptr(new dir_handle_data);
		dhd->fh_data = fhd;
		ret = (DIR*)dhd.get();
		insert_dir_handle(ret, dhd);

		DEBUG_PRINT(cpath);

		int readdir_rc = dhd->fh_data->file_system->readdir(cpath.c_str(), &(dhd->dir_entries));
		if(readdir_rc == 0) {
			dhd->dir_pointer = dhd->dir_entries.begin();
		} else {
			errno = readdir_rc;
			ret = NULL;
		}
	}

	OP_EXIT(opendir, (path));

	return ret;
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
