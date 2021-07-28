#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "libdefuse.h"

#define MAX_MOUNTS 16
#define DEFUSE_MOUNT_TYPE "defuse"
#define DEFUSE_MOUNT_TYPE_SIZE (sizeof(DEFUSE_MOUNT_TYPE) - 1)

struct mount_point_array {
	size_t length;
	struct mount_point_data* mounts[MAX_MOUNTS];
};

#define LOAD_TARGET(HANDLE, NAME, OBJECT, TARGET, REQUIRED) \
{ \
	if (OBJECT) { \
		dlerror(); \
		OBJECT->TARGET = dlsym(HANDLE, NAME); \
		const char* error = dlerror(); \
		if (error) { \
			DEBUG_PRINT(error); \
			if (REQUIRED) { \
				DEBUG_PRINT("REQUIRED FS FUNCTION NOT FOUND"); \
				DEBUG_PRINT(NAME); \
				dlclose(HANDLE); \
				free(OBJECT); \
				OBJECT = NULL; \
			} else { \
				OBJECT->TARGET = NULL; \
			} \
		} \
	} \
}

static inline ssize_t read_line(int fd, char* buf, size_t size)
{
	int rv = 0;
	char* p = buf;
	for(int i = 0; i < size; i++) {
		if((rv = real_read(fd, p, 1)) != 1) {
			return rv;
		}
		if(*p == '\n') {
			*p = 0;
			return p-buf;
		}
		p++;
	}
	return size;
}

static inline struct mount_point_data*
load_mount(const char* mount_path, const char* backend_path, const char* library_path)
{
	DEBUG_ENTER;
	struct mount_point_data* rv;

	DEBUG_PRINT(mount_path);
	DEBUG_PRINT(backend_path);
	DEBUG_PRINT(library_path);

	rv = (struct mount_point_data*)malloc(sizeof(struct mount_point_data));
	memset(rv, 0, sizeof(struct mount_point_data));
	rv->mount_path = strdup(mount_path);
	rv->backend_path = strdup(backend_path);
	rv->library_path = strdup(library_path);

	// Clear errors
	dlerror();

	// Load file system library
	void* handle = dlopen(library_path, RTLD_LAZY);
	if(handle != NULL) {
		LOAD_TARGET(handle, "usfs_init", rv, init, false);
		LOAD_TARGET(handle, "usfs_finalize", rv, finalize, false);
		LOAD_TARGET(handle, "usfs_open", rv, open, true);
		LOAD_TARGET(handle, "usfs_close", rv, close, true);
		LOAD_TARGET(handle, "usfs_read", rv, read, true);
		LOAD_TARGET(handle, "usfs_write", rv, write, true);
		LOAD_TARGET(handle, "usfs_fsync", rv, fsync, true);
		LOAD_TARGET(handle, "usfs_ftruncate", rv, ftruncate, true);
		LOAD_TARGET(handle, "usfs_truncate", rv, truncate, true);
		LOAD_TARGET(handle, "usfs_fgetattr", rv, fgetattr, true);
		LOAD_TARGET(handle, "usfs_getattr", rv, getattr, true);
		LOAD_TARGET(handle, "usfs_readlink", rv, readlink, true);
		LOAD_TARGET(handle, "usfs_unlink", rv, unlink, true);
		LOAD_TARGET(handle, "usfs_save", rv, save, false);
		LOAD_TARGET(handle, "usfs_restore", rv, restore, false);
		LOAD_TARGET(handle, "usfs_access", rv, access, true);
		if (rv != NULL && rv->init != NULL) {
			rv->init(mount_path, backend_path);
		}
	} else {
		DEBUG_PRINT(dlerror());
		free(rv);
		rv = NULL;
	}

	DEBUG_EXIT(0);
    return rv;
}

static inline void
load_mounts_from_file(const char* file, struct mount_point_array* mount_points)
{
	int rv = 0;
	char buffer[512];

	char backend[512];
	char mount_point[512];
	char library[512];

	int mounts_fd = real_open(file, O_RDONLY);
	if(mounts_fd < 0) {
		print_error_and_exit("Unable to open %s: %s\n", file, strerror(errno));
	}
	char* p;
	while((rv = read_line(mounts_fd, buffer, sizeof(buffer))) > 0) {
		if(rv > DEFUSE_MOUNT_TYPE_SIZE && memcmp(buffer, DEFUSE_MOUNT_TYPE, DEFUSE_MOUNT_TYPE_SIZE) == 0) {
			if(sscanf(buffer, "defuse %s", mount_point) != 1) {
				print_error_and_exit("Unable to retrieve mount point from: %s\n", buffer);
			}
			if((p = strstr(buffer, "backend=")) == NULL ||
				sscanf(p, "backend=%[^, ]", backend) == 0) {
				print_error_and_exit("Unable to retrieve backend from: %s\n", buffer);
			}
			if((p = strstr(buffer, "library=")) == NULL ||
				sscanf(p, "library=%[^, ]", library) == 0) {
				print_error_and_exit("Unable to retrieve library from: %s\n", buffer);
			}

			struct mount_point_data* mp = load_mount(mount_point, backend, library);
			if (mp == NULL) {
				print_error_and_exit("Error loading mount MP NULL: %s %s %s\n", mount_point, backend, library);
			}
			mp->index = mount_points->length;
			mount_points->mounts[mount_points->length] = mp;
			mount_points->length++;
		}
	}
	real_close(mounts_fd);
}

static inline void
load_mounts(struct mount_point_array* mount_points)
{
	static bool mounts_loaded_flag = false;
	if(mounts_loaded_flag == true) {
		return;
	}
	mounts_loaded_flag = true;
	DEBUG_ENTER;

	mount_points->length = 0;

	load_mounts_from_file("/proc/mounts", mount_points);
	
	// Restore any saved file handles
	restore_file_handles_from_shared_space();

	DEBUG_EXIT(0);
}

static inline struct mount_point_array* get_instance()
{
	static struct mount_point_array mp_instance;
	static bool mp_initialized = false;
	if (mp_initialized == false) {
		memset(&mp_instance, 0, sizeof(struct mount_point_array));
		mp_initialized = true;
		load_mounts(&mp_instance);
	}
	return &mp_instance;
}

void load_mounts_void() {
	load_mounts(get_instance());
}

int get_all_mounts(struct mount_point_data*** mpd)
{
	struct mount_point_array* mount_points = get_instance();
	(*mpd) = (struct mount_point_data**)mount_points->mounts;
	return mount_points->length;
}

struct mount_point_data*
find_mount_and_strip_path(char* path)
{
	struct mount_point_data* rv = NULL;
	struct mount_point_array* mount_points = get_instance();
	for(int i = 0; i < mount_points->length; i++) {
		char *mp_ptr = strstr(path, mount_points->mounts[i]->mount_path);
		if(mp_ptr == path) {
			DEBUG_ENTER;
			DEBUG_PRINT(path);
			size_t mount_len = strlen(mount_points->mounts[i]->mount_path);
			size_t path_len = strlen(path);
			if (path_len > 1) {
				memmove(path, path + mount_len, path_len - mount_len);
				path[path_len - mount_len] = 0;
				if(path[0] == 0) {
					path[0] = '/';
					path[1] = 0;
				}
				rv = mount_points->mounts[i];
				DEBUG_EXIT(0);
				break;
			}
		}
	}
	return rv;
}

struct mount_point_data* 
find_mount_from_path(const char* path)
{
	DEBUG_ENTER;
	struct mount_point_data* rv = NULL;

	DEBUG_PRINT(path);

	struct mount_point_array* mount_points = get_instance();
	for(int i = 0; i < mount_points->length; i++) {
		char *mp_ptr = strstr(path, mount_points->mounts[i]->mount_path);
		if(mp_ptr == path) {
			rv = mount_points->mounts[i];
			break;
		}
	}

	DEBUG_EXIT(0);
	return rv;
}
