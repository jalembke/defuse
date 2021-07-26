#ifndef _LDDEFUSE_H
#define _LDDEFUSE_H

#include <stdint.h>
#include <dirent.h>
#include <string>

#if !defined(DEFUSE_DO_REAL_OPEN) && !defined(DEFUSE_DO_NULL_OPEN) && !defined(DEFUSE_DO_NO_OPEN)
#error "-DDEFUSE_DO_REAL_OPEN or -DDEFUSE_DO_NULL_OPEN required"
#endif

#ifndef DEFUSE_MOUNT
#error "-DDEFUSE_MOUNT is required"
#endif

#ifndef DEFUSE_BACKEND
#error "-DDEFUSE_BACKEND is required"
#endif

#ifdef DEBUG_LDDEFUSE
    #include "debug.h"
#else
    #define DEBUG_PRINT(PRINT_DATA)
    #define DEBUG_ENTER
    #define DEBUG_EXIT(value)
    #define DEBUG_PRINT_BUFFER(BPTR, BSIZE)
#endif

#define OP_ENTER \
	FileSystemWrapper* fs = NULL; \
    std::string cpath; \
    resolve_path(path, cpath); \
    if((fs = find_mount_and_strip_path(cpath)) != NULL) { \
        DEBUG_ENTER;

#define AT_OP_ENTER \
	FileSystemWrapper* fs = NULL; \
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
FileSystemWrapper* find_mount_and_strip_path(std::string& path);
FileSystemWrapper* find_mount_from_path(const std::string& path);

// File Handle Functions
int insert_file_handle(int fd, file_handle_data_ptr& fhd);
int remove_file_handle(int fd);
struct file_handle_data* find_file_handle(int fd);
int duplicate_file_handle(int fd1, int fd2);
int save_file_handles_to_shared_space(void);
int restore_file_handles_from_shared_space(void);

// Directory Handle Functions
int insert_dir_handle(DIR* dirp, dir_handle_data_ptr& dhd);
int remove_dir_handle(DIR* dirp);
dir_handle_data* find_dir_handle(DIR* dirp);

#endif // _LDDEFUSE_H
