#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#include "libdefuse.h"
#include "glibc_ops.h"

// int execl(const char *path, const char *arg, ... /* (char  *) NULL */);
// int execlp(const char *file, const char *arg, ... /* (char  *) NULL */);
// int execle(const char *path, const char *arg, ... /*, (char *) NULL, char * const envp[] */);

int execv(const char *path, char *const argv[])
{
	DEBUG_ENTER;
	int rv = 0;
	save_file_handles_to_shared_space();
	rv = real_ops.execv(path, argv);
	DEBUG_EXIT(rv);
	return rv;
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
	save_file_handles_to_shared_space();
	return real_ops.execve(filename, argv, envp);
}

int execvp(const char *file, char *const argv[])
{
	save_file_handles_to_shared_space();
	return real_ops.execvp(file, argv);
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
	save_file_handles_to_shared_space();
	return real_ops.execvpe(file, argv, envp);
}

int fexecve(int fd, char *const argv[], char *const envp[])
{
	save_file_handles_to_shared_space();
	return real_ops.fexecve(fd, argv, envp);
}
