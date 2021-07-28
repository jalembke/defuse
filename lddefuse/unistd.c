#include <unistd.h>
#

#include "libdefuse.h"
#include "glibc_ops.h"

extern char **environ;

int access(const char *path, int mode) 
{   
	DEFUSE_OP(access, path, path, mode);
	return real_ops.access(path, mode);
}

int close(int fd) 
{
	DEFUSE_FD_OP(close, fd, fd);
	return real_ops.close(fd);
}

int execv(const char *path, char *const argv[])
{
	DEBUG_PRINT(path);
	save_file_handles_to_shared_space();

	// vfork may have been called - retrieve correct address to real execv
	int (*real_execv)(const char*, char *const[]) = (int (*)(const char*, char *const[]))load_real_op("execv");
	
	return real_execv(path, argv);
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
	DEBUG_PRINT(filename);
	save_file_handles_to_shared_space();
	
	// vfork may have been called - retrieve correct address to real execv
	int (*real_execve)(const char*, char *const[], char *const[]) = (int (*)(const char*, char *const[], char *const[]))load_real_op("execve");
	
	return real_execve(filename, argv, envp);
}

int execvp(const char *file, char *const argv[])
{
	DEBUG_PRINT(file);
	save_file_handles_to_shared_space();

	// vfork may have been called - retrieve correct address to real execv
	int (*real_execvp)(const char*, char *const[]) = (int (*)(const char*, char *const[]))load_real_op("execvp");

	return real_execvp(file, argv);
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
	DEBUG_PRINT(file);
	save_file_handles_to_shared_space();

	// vfork may have been called - retrieve correct address to real execv
	int (*real_execvpe)(const char*, char *const[], char *const[]) = (int (*)(const char*, char *const[], char *const[]))load_real_op("execvep");

	return real_execvpe(file, argv, envp);
}

int fexecve(int fd, char *const argv[], char *const envp[])
{
	save_file_handles_to_shared_space();

	// vfork may have been called - retrieve correct address to real execv
	int (*real_fexecve)(int, char *const[], char *const[]) = (int (*)(int, char *const[], char *const[]))load_real_op("fexecve");

	return real_fexecve(fd, argv, envp);
}

int fdatasync(int fd) 
{
	DEFUSE_FD_OP(fsync, fd, fd, 1);
	return real_ops.fdatasync(fd);
}

int fsync(int fd) 
{
	DEFUSE_FD_OP(fsync, fd, fd, 0);
	return real_ops.fsync(fd);
}

int ftruncate(int fd, off_t length)
{
	DEFUSE_FD_OP(ftruncate, fd, fd, length);
	return real_ops.ftruncate(fd, length);
}

off_t lseek(int fd, off_t offset, int whence)
{
	DEFUSE_FD_OP(lseek, fd, fd, offset, whence);
	return real_ops.lseek(fd, offset, whence);
}

ssize_t read(int fd, void *buf, size_t count) 
{
	DEFUSE_FD_OP(read, fd, fd, buf, count);
	return real_ops.read(fd, buf, count);
}

ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{
	DEFUSE_OP(readlink, path, path, buf, bufsiz);
	return real_ops.readlink(path, buf, bufsiz);
}

int truncate(const char *path, off_t length)
{
	DEFUSE_OP(truncate, path, path, length);
	return real_ops.truncate(path, length);
}

int unlink(const char *path) 
{   
	DEFUSE_OP(unlink, path, path);
	return real_ops.unlink(path);
}

ssize_t write(int fd, const void *buf, size_t count)
{
	DEFUSE_FD_OP(write, fd, fd, buf, count);
	return real_ops.write(fd, buf, count);
}
