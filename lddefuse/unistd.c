#include "libdefuse.h"
#include "glibc_ops.h"

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
	save_file_handles_to_shared_space();
	return real_ops.execv(path, argv);
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
