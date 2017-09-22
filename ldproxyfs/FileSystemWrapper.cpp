#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "fs_syscall.h"
#include "FileSystemWrapper.h"

int
FileSystemWrapper::open(const char* path, int flags, mode_t mode, uint64_t* ret_fh)
{
	int ret = FileSystemSyscall::open((xBackend + path).c_str(), flags, mode);
	if(-1 == ret) {
		ret = errno;

	} else {
		*ret_fh = ret;
		ret = 0;
	}
	return ret;
}

int
FileSystemWrapper::create(const char* path, mode_t mode, uint64_t* ret_fh)
{
	int ret = FileSystemSyscall::creat((xBackend + path).c_str(), mode);
	if(-1 == ret) {
		ret = errno;
	} else {
		*ret_fh = ret;
		ret = 0;
	}
	return ret;
}

int
FileSystemWrapper::getattr(const char* path, struct stat *stbuf, int flags)
{
	int ret = FileSystemSyscall::stat((xBackend + path).c_str(), stbuf);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::trunc(const char* path, off_t offset)
{
	int ret = FileSystemSyscall::truncate((xBackend + path).c_str(), offset);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int 
FileSystemWrapper::chown(const char* path, uid_t u, gid_t g)
{
	int ret = FileSystemSyscall::chown((xBackend + path).c_str(), u, g);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int 
FileSystemWrapper::chmod(const char* path, mode_t mode)
{
	int ret = FileSystemSyscall::chmod((xBackend + path).c_str(), mode);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::access(const char* path, int mode)
{
	int ret = FileSystemSyscall::access((xBackend + path).c_str(), mode);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int 
FileSystemWrapper::rename(const char* path, const char* path_to)
{
	int ret = FileSystemSyscall::rename((xBackend + path).c_str(), (xBackend + path_to).c_str());
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int 
FileSystemWrapper::unlink(const char* path)
{
	int ret = FileSystemSyscall::unlink((xBackend + path).c_str());
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::mkdir(const char* path, mode_t mode)
{
	int ret = FileSystemSyscall::mkdir((xBackend + path).c_str(), mode);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::readdir(const char* path, std::set<std::string> *buf)
{
	(*buf).clear();
	return 0;
}

int
FileSystemWrapper::rmdir(const char* path)
{
	int ret = FileSystemSyscall::rmdir((xBackend + path).c_str());
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int 
FileSystemWrapper::symlink(const char *from, const char* path_to)
{
	int ret = FileSystemSyscall::symlink(from, (xBackend + path_to).c_str());
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::statvfs(const char* path, struct statvfs *stbuf)
{
	int ret = FileSystemSyscall::statvfs((xBackend + path).c_str(), stbuf);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::sync()
{
	FileSystemSyscall::sync();
	return 0;
}

int
FileSystemWrapper::close(uint64_t fh)
{
	int ret = FileSystemSyscall::close(fh);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::read(uint64_t fh, char *buf, size_t size, off_t offset, size_t* bytes_read)
{
	int ret = FileSystemSyscall::pread(fh, buf, size, offset);
	if(-1 == ret) {
		ret = errno;
		*bytes_read = 0;
	} else {
		*bytes_read = ret;
		ret = 0;
	}
	return ret;
}

int
FileSystemWrapper::write(uint64_t fh, const char *buf, size_t size, off_t offset, size_t* bytes_written)
{
	int ret = FileSystemSyscall::pwrite(fh, buf, size, offset);
	if(-1 == ret) {
		ret = errno;
		*bytes_written = 0;
	} else {
		*bytes_written = ret;
		ret = 0;
	}
	return ret;
}

int
FileSystemWrapper::fsync(uint64_t fh)
{
	int ret = FileSystemSyscall::fsync(fh);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::ftrunc(uint64_t fh, off_t offset)
{
	int ret = FileSystemSyscall::ftruncate(fh, offset);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}

int
FileSystemWrapper::fgetattr(uint64_t fh, struct stat *sbuf)
{
	int ret = FileSystemSyscall::fstat(fh, sbuf);
	if(-1 == ret) {
		ret = errno;
	}
	return ret;
}
