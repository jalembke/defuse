#ifndef _GLIBC_OPS_H
#define _GLIBC_OPS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
//#include <selinux/selinux.h>

struct glibc_ops {

	/* POSIX functions */
	int (*open)(const char *path, int flags, ...);
    int (*open64)(const char *path, int flags, ...);
    int (*openat)(int dirfd, const char *path, int flags, ...);
    int (*openat64)(int dirfd, const char *path, int flags, ...);
    int (*creat)(const char *path, mode_t mode);
    int (*creat64)(const char *path, mode_t mode);
    int (*unlink)(const char *path);
    int (*unlinkat)(int dirfd, const char *path, int flags);
    int (*rename)(const char *oldpath, const char *newpath);
    int (*renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);
    ssize_t (*read)(int fd, void *buf, size_t count);
    ssize_t (*pread)(int fd, void *buf, size_t count, off_t offset);
    ssize_t (*readv)(int fd, const struct iovec *vector, int count);
    ssize_t (*write)(int fd, const void *buf, size_t count);
    ssize_t (*pwrite)(int fd, const void *buf, size_t count, off_t offset);
    ssize_t (*writev)(int fd, const struct iovec *vector, int count);
    int (*truncate)(const char *path, off_t length);
    int (*ftruncate)(int fd, off_t length);
    int (*fallocate)(int fd, off_t offset, off_t length);
    int (*close)(int fd);
	int (*__xstat)(int vers, const char *name, struct stat *buf);
	int (*__xstat64)(int vers, const char *file, struct stat64 *buf);
    int (*__fxstat)(int vers, int fd, struct stat *buf);
    int (*__fxstat64)(int vers, int fd, struct stat64 *buf);
	int (*__fxstatat)(int vers, int dirfd, const char *pathname, struct stat *buf, int flags);
	int (*__fxstatat64)(int vers, int dirfd, const char *pathname, struct stat64 *buf, int flags);
    int (*__lxstat)(int vers, const char *path, struct stat *buf);
    int (*__lxstat64)(int vers, const char *path, struct stat64 *buf);
    int (*futimesat)(int dirfd, const char *path, const struct timeval times[2]);
    int (*utimes)(const char *path, const struct timeval times[2]);
    int (*utime)(const char *path, const struct utimbuf *buf);
    int (*futimes)(int fd, const struct timeval times[2]);
    int (*dup)(int oldfd);
    int (*dup2)(int oldfd, int newfd);
    int (*dup3)(int oldfd, int newfd, int flags);
    int (*chown)(const char *path, uid_t owner, gid_t group);
    int (*fchown)(int fd, uid_t owner, gid_t group);
    int (*fchownat)(int fd, const char *path, uid_t owner, gid_t group, int flag);
    int (*lchown)(const char *path, uid_t owner, gid_t group);
    int (*chmod)(const char *path, mode_t mode);
    int (*fchmod)(int fd, mode_t mode);
    int (*fchmodat)(int fd, const char *path, mode_t mode, int flag);
	int (*lchmod)(const char *path, mode_t mode);
    int (*mkdir)(const char *path, mode_t mode);
    int (*mkdirat)(int dirfd, const char *path, mode_t mode);
    int (*rmdir)(const char *path);
    ssize_t (*readlink)(const char *path, char *buf, size_t bufsiz);
    ssize_t (*readlinkat)(int dirfd, const char *path, char *buf, size_t bufsiz);
    int (*symlink)(const char *oldpath, const char *newpath);
    int (*symlinkat)(const char *oldpath, int newdirfd, const char *newpath);
    int (*link)(const char *oldpath, const char *newpath);
    int (*linkat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags);
	DIR* (*opendir)(const char *name);
	DIR* (*fdopendir)(int fd);
    struct dirent* (*readdir)(DIR* dirp);
    struct dirent64* (*readdir64)(DIR* dirp);
    int (*readdir_r)(DIR *dirp, struct dirent *entry, struct dirent **result);
	int (*readdir64_r)(DIR* dirp, struct dirent *entry, struct dirent64 **result);
	int (*closedir)(DIR *dirp);
    int (*access)(const char *path, int mode);
    int (*faccessat)(int dirfd, const char *path, int mode, int flags);
    int (*flock)(int fd, int op);
    int (*fcntl)(int fd, int cmd, ...);
    void (*sync)(void);
    int (*fsync)(int fd);
    int (*fdatasync)(int fd);
    int (*fadvise)(int fd, off_t offset, off_t len, int advice);
    int (*fadvise64)(int fd, off64_t offset, off64_t len, int advice);
    int (*statfs)(const char *path, struct statfs *buf);
    int (*statfs64)(const char *path, struct statfs64 *buf);
    int (*fstatfs)(int fd, struct statfs *buf);
    int (*fstatfs64)(int fd, struct statfs64 *buf);
    int (*statvfs)(const char *path, struct statvfs *buf);
    int (*fstatvfs)(int fd, struct statvfs *buf);
	int (*__xmknod)(int vers, const char *path, mode_t mode, dev_t *dev);
    int (*__xmknodat)(int vers, int dirfd, const char *path, mode_t mode, dev_t dev);
    ssize_t (*sendfile)(int outfd, int infd, off_t *offset, size_t count);
    ssize_t (*sendfile64)(int outfd, int infd, off64_t *offset, size_t count);
    int (*setxattr)(const char *path, const char *name, const void *value, size_t size, int flags);
    int (*lsetxattr)(const char *path, const char *name, const void *value, size_t size, int flags);
    int (*fsetxattr)(int fd, const char *name, const void *value, size_t size, int flags);
    ssize_t (*getxattr)(const char *path, const char *name, void *value, size_t size);
    ssize_t (*lgetxattr)(const char *path, const char *name, void *value, size_t size);
    ssize_t (*fgetxattr)(int fd, const char *name, void *value, size_t size);
    ssize_t (*listxattr)(const char *path, char *list, size_t size);
    ssize_t (*llistxattr)(const char *path, char *list, size_t size);
    ssize_t (*flistxattr)(int fd, char *list, size_t size);
    int (*removexattr)(const char *path, const char *name);
    int (*lremovexattr)(const char *path, const char *name);
    int (*fremovexattr)(int fd, const char *name);
    void *(*mmap)(void *start, size_t length, int prot, int flags, int fd, off_t offset);
    int (*munmap)(void *start, size_t length);
    int (*msync)(void *start, size_t length, int flags);

	/* exec functions */
	int (*execl)(const char *path, const char *arg, ... /* (char  *) NULL */);
	int (*execlp)(const char *file, const char *arg, ... /* (char  *) NULL */);
    int (*execle)(const char *path, const char *arg, ... /*, (char *) NULL, char * const envp[] */);
    int (*execv)(const char *path, char *const argv[]);
	int (*execve)(const char *filename, char *const argv[], char *const envp[]);
    int (*execvp)(const char *file, char *const argv[]);
    int (*execvpe)(const char *file, char *const argv[], char *const envp[]);
	int (*fexecve)(int fd, char *const argv[], char *const envp[]);

	/* stdio functions */

    /* selinux operations */
    /*int (*getfscreatecon)(security_context_t *con);
    int (*getfilecon)(const char *path, security_context_t *con);
    int (*lgetfilecon)(const char *path, security_context_t *con);
    int (*fgetfilecon)(int fd, security_context_t *con);
    int (*setfscreatecon)(security_context_t con);
    int (*setfilecon)(const char *path, security_context_t con);
    int (*lsetfilecon)(const char *path, security_context_t con);
    int (*fsetfilecon)(int fd, security_context_t con);*/
};

extern struct glibc_ops real_ops;

void load_glibc_ops(void);

#endif // _GLIBC_OPS_H
