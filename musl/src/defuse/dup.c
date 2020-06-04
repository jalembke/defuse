#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "libdefuse.h"

static inline int dup_common(int oldfd, int newfd, int flags, bool do_real_dup) 
{
	DEBUG_ENTER;
    DEBUG_PRINT(oldfd << " " << newfd);

    int ret = 0;

	struct file_handle_data* old_fhd = find_file_handle(oldfd);
	struct file_handle_data* new_fhd = do_real_dup ? find_file_handle(newfd) : NULL;

    // If the caller is duping a descriptor over the top of an existing
    //    descriptor, then close the overwritten descriptor
    if(new_fhd != NULL) {
        close(newfd);
    }

    // Perform the real dup if necessary
    ret = do_real_dup ? real_dup3(oldfd, newfd, flags) : newfd;

    // Dup the file in the file handle map
	if(old_fhd != NULL) {
        if(ret != -1) {
			int dup_ret = duplicate_file_handle(oldfd, newfd);
			if(dup_ret != 0) {
				ret = -1;
				errno = dup_ret;
			}
		}
	}

	DEBUG_EXIT(ret);
}

int defuse_dup(int oldfd)
{
	int newfd = real_dup(oldfd);
	if(newfd != -1) {
		return dup_common(oldfd, newfd, 0, false);
	}
	return newfd;
}

int defuse_dup2(int oldfd, int newfd)
{
	return dup_common(oldfd, newfd, 0, true);
}

int defuse_dup3(int oldfd, int newfd, int flags)
{
	return dup_common(oldfd, newfd, flags, true);
}
