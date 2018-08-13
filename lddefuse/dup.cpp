#include <errno.h>
#include <unistd.h>

#include "lddefuse.h"
#include "glibc_ops.h"
#include "FileSystemWrapper.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
    extern "C" {
#endif

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
#if defined(DEFUSE_DO_NO_OPEN)
	if (old_fhd == NULL) {
		DEBUG_PRINT("REALDUP3: " << oldfd << " : " << newfd);
		ret = do_real_dup ? real_ops.dup3(oldfd, newfd, flags) : newfd;
	} else {
		close(newfd);
		ret = newfd;
	}
#else
    ret = do_real_dup ? real_ops.dup3(oldfd, newfd, flags) : newfd;
#endif

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
	return ret;
}

int dup(int oldfd) 
{
	int newfd = real_ops.dup(oldfd);
	if(newfd != -1) {
		return dup_common(oldfd, newfd, 0, false);
	}

	return newfd;
}

int dup2(int oldfd, int newfd) {
	return dup_common(oldfd, newfd, 0, true);
}

int dup3(int oldfd, int newfd, int flags) {
	return dup_common(oldfd, newfd, flags, true);
}

#ifdef __cplusplus
#endif
}
#pragma GCC visibility push(default)
