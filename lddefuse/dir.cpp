#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sstream>

#include "lddefuse.h"
#include "glibc_ops.h"
#include "file_handle_data.h"
#include "dir_handle_data.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

int mkdir(const char *path, mode_t mode)
{   
	int ret = 0;

	OP_ENTER;
	ret = fs->mkdir(cpath.c_str(), mode);
	if(ret != 0) {
		errno = ret;
		ret = -1;
	}
	OP_EXIT(mkdir, (path, mode));
    
    return ret;
}

int mkdirat(int dirfd, const char *path, mode_t mode)
{
	int ret = 0;

	AT_OP_ENTER;
	ret = fs->mkdir(cpath.c_str(), mode);
	if(ret != 0) {
		errno = ret;
		ret = -1;
	}
	OP_EXIT(mkdirat, (dirfd, path, mode));
    
    return ret;
}

int rmdir(const char *path)
{
	int ret = 0;

	OP_ENTER;
	ret = fs->rmdir(cpath.c_str());
	if(ret != 0) {
		errno = ret;
		ret = -1;
	}
	OP_EXIT(rmdir, (path));
    
    return ret;
}

struct dirent *readdir(DIR *dirp)
{
	struct dirent* ret = NULL;

	DIR_OP_ENTER;
	/*
	if(dhd->dir_pointer != dhd->dir_entries.end()) {
		static struct dirent local_dirent;
		struct stat local_stat;
		const std::string* local_path;
		int local_rv = 0;

		local_rv = dhd->fh_data->file_system->getLogicalPath(dhd->fh_data->file_handle, local_path);
		if(local_rv == 0) {
			std::stringstream dirent_path;
			dirent_path << (*local_path) << "/" << (*dhd->dir_pointer);
			local_rv = dhd->fh_data->file_system->getattr(dirent_path.str().c_str(), &local_stat, AT_SYMLINK_NOFOLLOW);

			if(local_rv == 0) {
				memset(&local_dirent, 0x00, sizeof(struct dirent));
				local_dirent.d_ino = local_stat.st_ino;
				strncpy(local_dirent.d_name, dhd->dir_pointer->c_str(), sizeof(local_dirent.d_name));
				ret = &local_dirent;
				dhd->dir_pointer++;
			} else {
				errno = local_rv;
			}
		} else {
			errno = local_rv;
		}
	}
	*/
	OP_EXIT(readdir, (dirp));

	return ret;
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
