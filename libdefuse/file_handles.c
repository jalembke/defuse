#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

#include "libdefuse.h"

#define MAX_FILES 1024

struct file_handle_data_internal {
	struct file_handle_data f_data;
	int use_count;
};
struct file_handle_meta {
	int use_position;
};
struct file_handle_table {
	struct file_handle_data_internal* file_handles[MAX_FILES];
	struct file_handle_meta file_meta[MAX_FILES];
	int used_file_descriptors[MAX_FILES];
	int used_file_descriptor_pos;
};

extern void load_mounts_void();
extern int get_all_mounts(struct mount_point_data*** mpd);

static inline struct file_handle_table* get_instance()
{
	static struct file_handle_table fht_instance;
	static bool fht_initialized = false;
	if(!fht_initialized) {
		memset(&fht_instance, 0, sizeof(struct file_handle_table));
		fht_initialized = true;
	}
	return &fht_instance;
}

static inline int file_handle_use_count_internal(const struct file_handle_data_internal* fhd)
{
	return fhd == NULL ? 0 : fhd->use_count;
}

int file_handle_use_count(struct file_handle_data* fhd)
{
	return file_handle_use_count_internal((const struct file_handle_data_internal*)fhd);
}

static inline struct file_handle_data_internal* allocate_file_handle(const struct file_handle_data* fhd)
{
	struct file_handle_data_internal* fhdi = (struct file_handle_data_internal*)malloc(sizeof(struct file_handle_data_internal));
	memcpy(&fhdi->f_data, fhd, sizeof(struct file_handle_data));
	fhdi->use_count = 0;
	return fhdi;
}

static inline void link_file_descriptor(struct file_handle_table* fht, int fd)
{
	fht->file_meta[fd].use_position = fht->used_file_descriptor_pos;
	fht->used_file_descriptors[fht->used_file_descriptor_pos] = fd;
	fht->used_file_descriptor_pos++;
}

static inline void unlink_file_descriptor(struct file_handle_table* fht, int fd)
{
	if(fht->used_file_descriptor_pos > 1) {
		fht->used_file_descriptors[fht->file_meta[fd].use_position] = fht->used_file_descriptor_pos-1;
		fht->file_meta[fht->used_file_descriptor_pos - 1].use_position = fht->file_meta[fd].use_position;
	}
	fht->used_file_descriptor_pos--;
	fht->file_handles[fd] = NULL;
}

int
insert_file_handle(int fd, struct file_handle_data* fhd)
{
	DEBUG_ENTER;
	int rv = 0;
	if (fd >= MAX_FILES) {
		rv = EBADF;
	} else {
		struct file_handle_table* fht = get_instance();
		if(file_handle_use_count_internal(fht->file_handles[fd]) > 0) {
			rv = EEXIST;
		} else {
			fht->file_handles[fd] = allocate_file_handle(fhd);
			fht->file_handles[fd]->use_count++;
			link_file_descriptor(fht, fd);
		}
	}
	DEBUG_EXIT(rv);
    return rv;
}

int
remove_file_handle(int fd)
{
	DEBUG_ENTER;
	int rv = 0;
	if (fd >= MAX_FILES) {
		rv = EBADF;
	} else {
		struct file_handle_table* fht = get_instance();
		if (file_handle_use_count_internal(fht->file_handles[fd]) == 0) {
			rv = ENOENT;
		} else {
			fht->file_handles[fd]->use_count--;
			if(fht->file_handles[fd]->use_count == 0) {
				DEBUG_PRINT("FREE HANDLE");
				free(fht->file_handles[fd]);
			}
			unlink_file_descriptor(fht, fd);
		}
	}
	DEBUG_EXIT(rv);
    return rv;
}

static inline bool file_handle_exists(int fd)
{
	if (fd >= MAX_FILES) {
		return false;
	}
	struct file_handle_table* fht = get_instance();
	return file_handle_use_count_internal(fht->file_handles[fd]) > 0;
}

struct file_handle_data*
find_file_handle(int fd)
{
	struct file_handle_data* rv = NULL;
	if (fd >= MAX_FILES) {
		goto out;
	}
	load_mounts_void();
	struct file_handle_table* fht = get_instance();
	if (file_handle_use_count_internal(fht->file_handles[fd]) == 0) {
		goto out;
	}
	DEBUG_ENTER;
	rv = &(fht->file_handles[fd]->f_data);
	DEBUG_EXIT(rv ? rv->file_handle : 0);
out:
    return rv;
}

int 
duplicate_file_handle(int fd1, int fd2, struct file_handle_data** oldfhd)
{
	DEBUG_ENTER;
	int rv = 0;
	(*oldfhd) = NULL;
	if (fd1 >= MAX_FILES || fd2 >= MAX_FILES) {
		rv = EBADF;
	} else {
		struct file_handle_table* fht = get_instance();
		if (fht->file_handles[fd2] != NULL) {
			(*oldfhd) = &fht->file_handles[fd2]->f_data;
		}
		if (file_handle_use_count_internal(fht->file_handles[fd1]) > 0) {
			fht->file_handles[fd2] = fht->file_handles[fd1];
			fht->file_handles[fd1]->use_count++;
			if((*oldfhd) == NULL) {
				link_file_descriptor(fht, fd2);
			}
		}
	}
	DEBUG_EXIT(rv);
    return rv;
}

static inline off_t copy_with_offset(void* target, const void* source, size_t bytes, off_t offset)
{
	memcpy(((char*)target) + offset, source, bytes);
	return bytes;
}

struct shared_space_mount_point_data {
	uint64_t index;
	uint64_t length;
};
struct shared_space_file_data {
	uint64_t file_handle;
	uint64_t file_descriptor;
	uint64_t mount_point_index;
};

int save_file_handles_to_shared_space()
{
	DEBUG_ENTER;
	int rv = 0;
	struct shared_space_mount_point_data mp_data;
	struct shared_space_file_data f_data;
	struct mount_point_data** mpd = NULL;
	struct file_handle_table* fht = get_instance();
	uint64_t file_count = (uint64_t)fht->used_file_descriptor_pos;

	if(file_count > 0) {
		uint64_t mount_points_length = (uint64_t)get_all_mounts(&mpd);

		// Determine size of mount point section of shared space
		//   Mount point section consists of an array of mount point strings along with their index
		//   The header for the mount point section begins with the count of entries (uint64_t)
		int shared_space_size = 0;
		uint64_t mount_point_file_size = sizeof(uint64_t);

		for(int i = 0; i < mount_points_length; i++) {
			mount_point_file_size += (sizeof(struct shared_space_mount_point_data) + strlen(mpd[i]->mount_path));
		}
		// Determine the total size of the shared space
		shared_space_size = mount_point_file_size + sizeof(file_count) + file_count * sizeof(struct shared_space_file_data);

		DEBUG_PRINT("MPCOUNT:");
		DEBUG_PRINT_INT(mount_points_length);
		DEBUG_PRINT("MPFSIZE:");
		DEBUG_PRINT_INT(mount_point_file_size);
		DEBUG_PRINT("FHSIZE:");
		DEBUG_PRINT_INT(shared_space_size);
		DEBUG_PRINT("FHCOUNT:");
		DEBUG_PRINT_INT(file_count);

		// Retrieve the shared space pointer and write the file descriptor data
		void* shared_space_ptr = init_shared_space(shared_space_size);
		if (shared_space_ptr != NULL) {
			off_t copy_offset = 0;
			copy_offset += copy_with_offset(shared_space_ptr, &mount_points_length, sizeof(mount_points_length), copy_offset);
			for(int i = 0; i < mount_points_length; i++) {
				int mp_path_len = strlen(mpd[i]->mount_path);
				mp_data.index = mpd[i]->index;
				mp_data.length = mp_path_len+1;
				copy_offset += copy_with_offset(shared_space_ptr, &mp_data, sizeof(mp_data), copy_offset);
				copy_offset += copy_with_offset(shared_space_ptr, mpd[i]->mount_path, mp_path_len, copy_offset);
				((char*)shared_space_ptr)[copy_offset++] = 0;
			}
			copy_offset += copy_with_offset(shared_space_ptr, &file_count, sizeof(file_count), copy_offset);
			for(int i = 0; i < fht->used_file_descriptor_pos; i++) {
				struct file_handle_data* fhd = &fht->file_handles[fht->used_file_descriptors[i]]->f_data;

				// Call client library save routine if defined
				if(fhd->mount->save) {
					fhd->mount->save(fhd->file_handle);
				}

				f_data.file_handle = fhd->file_handle;
				f_data.file_descriptor = fht->used_file_descriptors[i];
				f_data.mount_point_index = fhd->mount->index;
				copy_offset += copy_with_offset(shared_space_ptr, &f_data, sizeof(f_data), copy_offset);
			}
			release_shared_space();
		} else {
			DEBUG_PRINT("NO SHARED PTR");
			rv = -1;
		}
	}
	DEBUG_EXIT(rv);
	return rv;
}

int restore_file_handles_from_shared_space()
{
	DEBUG_ENTER;
	int rv = 0;
	struct shared_space_mount_point_data* mp_data = NULL;
	struct shared_space_file_data* f_data = NULL;

	void* shared_space_ptr = get_shared_space();
	if(shared_space_ptr) {
		char* current_ptr = (char*)shared_space_ptr;
		uint64_t* mount_point_count = (uint64_t*)shared_space_ptr;

		// Retrieve the mount points
		DEBUG_PRINT_INT(*mount_point_count * sizeof(char*));
		char** mount_paths = (char**)malloc(*mount_point_count * sizeof(char*));
		current_ptr += sizeof(uint64_t);
		for(int i = 0; i < *mount_point_count; i++) {
			mp_data = (struct shared_space_mount_point_data*)current_ptr;
			current_ptr += sizeof(struct shared_space_mount_point_data);
			mount_paths[i] = current_ptr;
			current_ptr += mp_data->length;
		}

		// Retrieve the file handles
		uint64_t* file_count = (uint64_t*)current_ptr;
		current_ptr += sizeof(uint64_t);
		for(int i = 0; i < *file_count; i++) {
			f_data = (struct shared_space_file_data*)current_ptr;
			current_ptr += sizeof(struct shared_space_file_data);
			if(f_data->mount_point_index < *mount_point_count) {
				struct mount_point_data* mount_point = find_mount_from_path(mount_paths[f_data->mount_point_index]);
				if(mount_point != NULL) {
					int real_fd = (int)f_data->file_descriptor;

					// Don't add it if it already exists
					if(!file_handle_exists(real_fd)) {
						uint64_t fh = f_data->file_handle;

						// Call client library restore routine if defined
						if(mount_point->restore) {
							mount_point->restore(fh);
						}

						struct file_handle_data fhd;
						fhd.file_handle = fh;
						fhd.mount = mount_point;
						rv = insert_file_handle(real_fd, &fhd);
					}
				}
			}
		}
		free(mount_paths);
		release_shared_space();
	} else {
		DEBUG_PRINT("NO SHARED PTR");
		rv = -1;
	}
	DEBUG_EXIT(rv);
}
