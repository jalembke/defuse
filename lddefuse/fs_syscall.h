#ifndef _FILESYSTEMSYSCALL_H
#define _FILESYSTEMSYSCALL_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include <utime.h>

#define STATIC_SYSCALL

#ifdef STATIC_SYSCALL
#include <sys/syscall.h>
#endif

class FileSystemSyscall {

    public:

        static inline int creat(const char *pathname, mode_t mode) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_creat, pathname, mode);
            #else
                return ::creat(pathname, mode);
            #endif
        }

        static inline int unlink(const char *pathname) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_unlink, pathname);
            #else
                return ::unlink(pathname);
            #endif
        }

        static inline int open(const char *pathname, int flags) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_open, pathname, flags);
            #else
                return ::open(pathname, flags);
            #endif
        }

        static inline int open(const char *pathname, int flags, mode_t mode) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_open, pathname, flags, mode);
            #else
                return ::open(pathname, flags, mode);
            #endif
        }

        static inline int close(int fd) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_close, fd);
            #else
                return ::close(fd);
            #endif
        }

        static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_pread64, fd, buf, count, offset);
            #else
                return ::pread(fd, buf, count, offset);
            #endif
        }

        static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_pwrite64, fd, buf, count, offset);
            #else
                return ::pwrite(fd, buf, count, offset);
            #endif
        }

        static inline int stat(const char *path, struct stat *buf) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_stat, path, buf);
            #else
                return ::stat(path, buf);
            #endif
        }

        static inline int fstat(int fd, struct stat *buf) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_fstat, fd, buf);
            #else
                return ::fstat(fd, buf);
            #endif
        }

        static inline int lstat(const char* path, struct stat *buf) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_lstat, path, buf);
            #else
                return ::lstat(path, buf);
            #endif
        }

		static inline int statvfs(const char *path, struct statvfs *buf) {
			return ::statvfs(path, buf);
		}

        static inline int mkdir(const char *pathname, mode_t mode) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_mkdir, pathname, mode);
            #else
                return ::mkdir(pathname, mode);
            #endif
        }

        static inline int rmdir(const char *pathname) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_rmdir, pathname);
            #else
                return ::rmdir(fd);
            #endif
        }

		static inline int sync() {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_sync);
            #else
                return ::sync();
            #endif
        }

        static inline int fsync(int fd) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_fsync, fd);
            #else
                return ::fsync(fd);
            #endif
        }

        static inline int truncate(const char *path, off_t length) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_truncate, path, length);
            #else
                return ::truncate(path, length);
            #endif
        } 

        static inline int ftruncate(int fd, off_t length) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_ftruncate, fd, length);
            #else
                return ::ftruncate(fd, length);
            #endif
        }

        static inline int chown(const char *path, uid_t owner, gid_t group) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_chown, path, owner, group);
            #else
                return ::chown(path, owner, group);
            #endif
        }

        static inline int chmod(const char *path, mode_t mode) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_chmod, path, mode);
            #else
                return ::chmod(path, mode);
            #endif
        }

        static inline int access(const char *pathname, int mode) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_access, pathname, mode);
            #else
                return ::access(pathname, mode);
            #endif
        }

        static inline int rename(const char *oldpath, const char *newpath) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_rename, oldpath, newpath);
            #else
                return ::access(oldpath, newpath);
            #endif
        }

        static inline int utime(const char *filename, const struct utimbuf *times) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_utime, filename, times);
            #else
                return ::utime(filename, times);
            #endif
        }

        static inline ssize_t readlink(const char *path, char *buf, size_t bufsiz) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_readlink, path, buf, bufsiz);
            #else
                return ::readlink(path, buf, bufsize);
            #endif
        }

		static inline int symlink(const char *target, const char *linkpath) {
            #ifdef STATIC_SYSCALL
                return syscall(SYS_symlink, target, linkpath);
            #else
                return ::symlink(target, linkpath);
            #endif
        }

		static inline int fcntl_lock_unlock(int fd, int cmd, struct flock* lockp) {
            #ifdef STATIC_SYSCALL
                return ::fcntl(fd, cmd, lockp);
                //return syscall(SYS_fcntl, fd, cmd, lockp);
            #else
                return ::fcntl(fd, cmd, lockp);
            #endif
		}
};

#endif // _FILESYSTEMSYSCALL_H
