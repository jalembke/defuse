#ifndef _DEFUSE_H
#define _DEFUSE_H

#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>

struct mount_point_data
{
	int index;
	char* mount_path;
	char* backend_path;
	char* library_path;

	void (*init) (const char*, const char*);
	void (*finalize) (void);
	int (*access) (const char*, int);
	int (*close) (uint64_t);
	int (*getattr) (const char*, struct stat*, int);
	int (*fsync) (uint64_t, int);
	int (*ftruncate) (uint64_t, off_t);
	int (*truncate) (const char*, off_t);
	int (*fgetattr) (uint64_t, struct stat*);
	int (*open) (const char *, int, mode_t, uint64_t*);
	int (*read) (uint64_t, char*, size_t, off_t, size_t*);
	int (*write) (uint64_t, const char*, size_t, off_t, size_t*);
	int (*unlink) (const char*);
	int (*readlink) (const char*, char*, size_t, size_t*);
	int (*save) (uint64_t);
	int (*restore) (uint64_t);
};

struct file_handle_data 
{
	uint64_t file_handle;
	const struct mount_point_data* mount;
};
// #include "dir_handle_data.h"

#define DEBUG_DEFUSE

#ifdef DEBUG_DEFUSE
    #include "libdefuse_debug.h"
#else
    #define DEBUG_PRINT(PRINT_DATA)
    #define DEBUG_PRINT_INT(PRINT_DATA)
    #define DEBUG_ENTER
    #define DEBUG_EXIT(value)
    #define DEBUG_PRINT_BUFFER(BPTR, BSIZE)
#endif

#define DEFUSE_OP(OP, PATH, ...) \
	{ \
		struct mount_point_data* mp_data = NULL; \
		char* cpath = resolve_path(PATH); \
		if((mp_data = find_mount_and_strip_path(cpath)) != NULL) { \
			return defuse_##OP(mp_data, cpath, __VA_ARGS__); \
		} \
	}

#define DEFUSE_FD_OP(OP, FD, ...) \
	{ \
		struct file_handle_data* fhd = find_file_handle(FD); \
		if(fhd != NULL) { \
			return defuse_##OP(fhd, __VA_ARGS__); \
		} \
	}

/*
#define AT_OP_ENTER \
	FileSystemWrapper* fs = NULL; \
    std::string cpath; \
    resolve_path_at(dirfd, path, cpath); \
    if((fs = find_mount_and_strip_path(cpath)) != NULL) { \
        DEBUG_ENTER;
    }
*/
// Real operations used internally by libdefuse
extern int real_open(const char *filename, int flags, ...);
extern void* real_mmap(void *start, size_t len, int prot, int flags, int fd, off_t off);
extern ssize_t real_read(int fd, void *buf, size_t count);
extern int real_close(int fd);
extern off_t real_lseek(int fd, off_t offset, int whence);
extern int real_ftruncate(int fd, off_t length);

// Syscall Operations
int defuse_access(const struct mount_point_data* mp, const char* cpath, const char* filename, int mode);
int defuse_open(const struct mount_point_data* mp, const char* cpath, const char* filename, int flags, mode_t mode);
int defuse_close(const struct file_handle_data* fhd, int fd);
int defuse_getattr(const struct mount_point_data* mp, const char* cpath, const char* pathname, struct stat* statbuf, int flags);
ssize_t defuse_readlink(const struct mount_point_data* mp, const char* cpath, const char* pathname, char* buf, size_t bufsiz);
ssize_t defuse_read(const struct file_handle_data* fhd, int fd, void* buf, size_t count);
ssize_t defuse_write(const struct file_handle_data* fhd, int fd, const void* buf, size_t count);
int defuse_fgetattr(const struct file_handle_data* fhd, int fd, struct stat* statbuf);
int defuse_fsync(const struct file_handle_data* fhd, int fd, int data_sync);
int defuse_unlink(const struct mount_point_data*mp, const char* cpath, const char* path);
int defuse_truncate(const struct mount_point_data* mp, const char* cpath, const char* path, off_t length);
int defuse_ftruncate(const struct file_handle_data* fhd, int fd, off_t length);
int defuse_lseek(const struct file_handle_data* fhd, int fd, off_t offset, int whence);
int defuse_dup(int oldfd);
int defuse_dup2(int oldfd, int newfd);
int defuse_dup3(int oldfd, int newfd, int flags);

// Mount Point Functions
void load_mounts_void();
struct mount_point_data* find_mount_and_strip_path(char* path);
struct mount_point_data* find_mount_from_path(const char* path);

// File Handle Functions
int file_handle_use_count(struct file_handle_data* fhd);
int insert_file_handle(int fd, struct file_handle_data* fhd);
int remove_file_handle(int fd);
struct file_handle_data* find_file_handle(int fd);
int duplicate_file_handle(int fd1, int fd2, struct file_handle_data** old_fhd);
int save_file_handles_to_shared_space(void);
int restore_file_handles_from_shared_space(void);

// Shared Space Funnctions
void* get_shared_space();
void* init_shared_space(size_t size);
void release_shared_space();

/*
// Directory Handle Functions
int insert_dir_handle(DIR* dirp, dir_handle_data_ptr& dhd);
int remove_dir_handle(DIR* dirp);
dir_handle_data* find_dir_handle(DIR* dirp);
*/

// Utility Functions
void* memdup(void* ptr, size_t size);
char* resolve_path(const char *path);
void dfprintf(const char* format, ...);
void print_error_and_exit(const char* format, ...);

#endif // __DEFUSE_H
