#include <errno.h>
#include <string.h>
#include <map>

#include "lddefuse.h"
#include "file_handle_data.h"
#include "FileSystemWrapper.h"
#include "shared_space.h"

typedef std::map<int, file_handle_data_ptr> file_handle_map;

static inline file_handle_map& get_instance()
{
	static file_handle_map file_handles;
	return file_handles;
}

file_handle_data::~file_handle_data()
{
	DEBUG_ENTER;

	//const std::string* path = NULL;
	//file_system->getLogicalPath(file_handle, path);
	//DEBUG_PRINT(*path);	

	int rv = file_system->close(file_handle);
	DEBUG_EXIT(rv);
}

static inline bool
file_handle_exists(int fd)
{
	file_handle_map& file_handles = get_instance();
	return file_handles.find(fd) != file_handles.end();
}

int
insert_file_handle(int fd, file_handle_data_ptr& fhd)
{
	DEBUG_ENTER;
	int rv = 0;
	
	file_handle_map& file_handles = get_instance();
	std::map<int, file_handle_data_ptr>::iterator itr = file_handles.find(fd);
	if(itr != file_handles.end()) {
		rv = EEXIST;
	} else {
		file_handles.insert(std::make_pair(fd, fhd));
	}

	DEBUG_EXIT(rv);
    return rv;
}

int
remove_file_handle(int fd)
{
	int rv = 0;

	file_handle_map& file_handles = get_instance();
	std::map<int, file_handle_data_ptr>::iterator itr = file_handles.find(fd);
	if(itr == file_handles.end()) {
		rv = ENOENT;
	} else {
		DEBUG_ENTER;
		file_handles.erase(itr);
		DEBUG_EXIT(rv);
	}

    return rv;
}

struct file_handle_data*
find_file_handle(int fd)
{
	struct file_handle_data* rv = NULL;

	load_mounts();
	file_handle_map& file_handles = get_instance();
	std::map<int, file_handle_data_ptr>::iterator itr = file_handles.find(fd);
	if(itr != file_handles.end()) {
		DEBUG_ENTER;
		rv = itr->second.get();
		DEBUG_EXIT(rv);
	}

    return rv;
}

int 
duplicate_file_handle(int fd1, int fd2)
{
	DEBUG_ENTER;
	int rv = 0;

	file_handle_map& file_handles = get_instance();
	std::map<int, file_handle_data_ptr>::iterator itr = file_handles.find(fd1);
	if(itr == file_handles.end()) {
		rv = EBADF;
	} else {
		file_handles.insert(std::pair<int, file_handle_data_ptr >(fd2, itr->second));
	}

	DEBUG_EXIT(rv);
    return rv;
}

int get_file_handle_ref_count(int fd)
{
	DEBUG_ENTER;
	int rv = 0;

	file_handle_map& file_handles = get_instance();
	std::map<int, file_handle_data_ptr>::iterator itr = file_handles.find(fd);
	if(itr != file_handles.end()) {
		rv = itr->second.use_count();
	}

	DEBUG_EXIT(rv);
    return rv;
}

static inline off_t copy_with_offset(void* target, const void* source, size_t bytes, off_t offset)
{
	memcpy(((char*)target) + offset, source, bytes);
	return bytes;
}

typedef struct {
	uint64_t index;
	uint64_t length;
} shared_space_mount_point_data;

typedef struct {
	uint64_t file_handle;
	uint64_t file_descriptor;
	uint64_t mount_point_index;
} shared_space_file_data;

int save_file_handles_to_shared_space()
{
	DEBUG_ENTER;
	int rv = 0;
	file_handle_map& file_handles = get_instance();

	// Determine size of mount point section of shared space
	//   Mount point section consists of an array of mount point strings along with their index
	//   The header for the mount point section begins with the count of entries (uint64_t)
	int shared_space_size = 0;
	uint64_t mount_point_file_size = sizeof(uint64_t);
	uint64_t mount_point_index = 0;
	std::map<std::string, uint64_t> mount_points;
	for(std::map<int, file_handle_data_ptr>::iterator itr = file_handles.begin(); itr != file_handles.end(); itr++) {
		const std::string& mount_point_str = itr->second->file_system->getMountPoint();
		if(mount_points.find(mount_point_str) == mount_points.end()) {
			mount_point_file_size += (sizeof(shared_space_mount_point_data) + mount_point_str.length());
			mount_points.insert(std::make_pair(mount_point_str, mount_point_index++));
		}
	}

	// Determine the total size of the shared space
	uint64_t file_count = file_handles.size();
	shared_space_size = mount_point_file_size + sizeof(file_count) + file_count * sizeof(shared_space_file_data);

	DEBUG_PRINT("MPCOUNT: " << mount_point_index);
	DEBUG_PRINT("MPFSIZE: " << mount_point_file_size);
	DEBUG_PRINT("FHSIZE: " << shared_space_size);
	DEBUG_PRINT("FHCOUNT: " << file_count);

	// Retrieve the shared space pointer and write the file descriptor data
	void* shared_space_ptr = shared_space::init(shared_space_size);
	off_t copy_offset = 0;
	copy_offset += copy_with_offset(shared_space_ptr, &mount_point_index, sizeof(mount_point_index), copy_offset);
	for(std::map<std::string, uint64_t>::iterator mp_itr = mount_points.begin(); mp_itr != mount_points.end(); mp_itr++) {
		shared_space_mount_point_data mp_data = { mp_itr->second, mp_itr->first.length() };
		copy_offset += copy_with_offset(shared_space_ptr, &mp_data, sizeof(mp_data), copy_offset);
		copy_offset += copy_with_offset(shared_space_ptr, mp_itr->first.c_str(), mp_itr->first.length(), copy_offset);
	}
	copy_offset += copy_with_offset(shared_space_ptr, &file_count, sizeof(file_count), copy_offset);
	for(std::map<int, file_handle_data_ptr>::iterator itr = file_handles.begin(); itr != file_handles.end(); itr++) {
		shared_space_file_data f_data = { itr->second->file_handle, (uint64_t)itr->first, mount_points[itr->second->file_system->getMountPoint()] };
		copy_offset += copy_with_offset(shared_space_ptr, &f_data, sizeof(f_data), copy_offset);
	}
	DEBUG_EXIT(rv);
	return rv;
}

int restore_file_handles_from_shared_space()
{
	DEBUG_ENTER;
	int rv = 0;

	void* shared_space_ptr = shared_space::get();
	if(shared_space_ptr) {
		char* current_ptr = (char*)shared_space_ptr; 


		// Retrieve the mount points
		uint64_t* mount_point_count = (uint64_t*)current_ptr;
		current_ptr += sizeof(uint64_t);
		std::map<uint64_t, std::string> mount_points;
		for(int i = 0; i < *mount_point_count; i++) {
			shared_space_mount_point_data* mount_point_data_ptr = (shared_space_mount_point_data*)current_ptr;
			current_ptr += sizeof(shared_space_mount_point_data);
			char* mount_point_str = (char*)current_ptr;
			current_ptr += mount_point_data_ptr->length;
			mount_points.insert(std::make_pair(mount_point_data_ptr->index, std::string(mount_point_str, mount_point_data_ptr->length)));
		}
		// Retrieve the file handles
		uint64_t* file_count = (uint64_t*)current_ptr;
		current_ptr += sizeof(uint64_t);
		for(int i = 0; i < *file_count; i++) {
			shared_space_file_data* file_data_ptr = (shared_space_file_data*)current_ptr;
			current_ptr += sizeof(shared_space_file_data);
			std::map<uint64_t, std::string>::iterator mount_point_itr = mount_points.find(file_data_ptr->mount_point_index);
			if(mount_point_itr != mount_points.end()) {
				FileSystemWrapper* fs = find_mount_from_path(mount_point_itr->second);
				if(fs != NULL) {
					int real_fd = (int)file_data_ptr->file_descriptor;

					// Don't add it if it already exists
					if(!file_handle_exists(real_fd)) {
						uint64_t fh = file_data_ptr->file_handle;
						DEBUG_PRINT(fs << " : " << fh << " : " << real_fd);
						file_handle_data_ptr fhd = file_handle_data_ptr(new file_handle_data(fs, fh, real_fd));
						rv = insert_file_handle(real_fd, fhd);
					}
				}
			}
		}
	} else {
		DEBUG_PRINT("NO SHARED PTR");
	}
	DEBUG_EXIT(rv);
}

