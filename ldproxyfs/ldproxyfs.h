#ifndef __LDPROXYFS_H
#define __LDPROXYFS_H

#include <stdint.h>
#include <dirent.h>
#include <string>

#include "file_handle_data.h"
#include "dir_handle_data.h"

class HybridFileSystem;

#if !defined(PROXYFS_DO_REAL_OPEN) && !defined(PROXYFS_DO_NULL_OPEN)
#error "-DPROXYFS_DO_REAL_OPEN or -DPROXYFS_DO_NULL_OPEN required"
#endif

#ifndef PROXYFS_MOUNT
#error "-DPROXYFS_MOUNT is required"
#endif

#ifndef PROXYFS_BACKEND_STRING
#error "-DPROXYFS_BACKEND_STRING is required"
#endif

#ifdef DEBUG_LDPROXYFS
    #include "debug.h"
#else
    #define DEBUG_PRINT(PRINT_DATA)
    #define DEBUG_ENTER
    #define DEBUG_EXIT(value)
    #define DEBUG_PRINT_BUFFER(BPTR, BSIZE)
#endif

#define OP_ENTER \
	HybridFileSystem* fs = NULL; \
    std::string cpath; \
    resolve_path(path, cpath); \
    if((fs = find_mount_and_strip_path(cpath)) != NULL) { \
        DEBUG_ENTER;

#define AT_OP_ENTER \
	HybridFileSystem* fs = NULL; \
    std::string cpath; \
    resolve_path_at(dirfd, path, cpath); \
    if((fs = find_mount_and_strip_path(cpath)) != NULL) { \
        DEBUG_ENTER;

#define FD_OP_ENTER \
	file_handle_data* fhd = find_file_handle(fd); \
	if(fhd != NULL) { \
        DEBUG_ENTER;

#define DIR_OP_ENTER \
	dir_handle_data* dhd = find_dir_handle(dirp); \
    if(dhd != NULL) { \
        DEBUG_ENTER;

#define OP_EXIT(realfunc, realargs) \
        DEBUG_EXIT(ret); \
    } else { \
        ret = real_ops.realfunc realargs; \
    }

// Mount Point Functions
void load_mounts(void);
HybridFileSystem* find_mount_and_strip_path(std::string& path);

// File Handle Functions
int insert_file_handle(int fd, file_handle_data_ptr& fhd);
int remove_file_handle(int fd);
struct file_handle_data* find_file_handle(int fd);
int duplicate_file_handle(int fd1, int fd2);

// Directory Handle Functions
int insert_dir_handle(DIR* dirp, dir_handle_data_ptr& dhd);
int remove_dir_handle(DIR* dirp);
dir_handle_data* find_dir_handle(DIR* dirp);

// Path Resolution
void resolve_path(const char *p, std::string& path);
void resolve_path_at(int dirfd, const char *p, std::string& path);

#endif // __LDPROXYFS_H
