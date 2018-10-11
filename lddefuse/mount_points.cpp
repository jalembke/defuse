#include <stdlib.h>
#include <errno.h>
#include <strings.h>

#include <string>
#include <map>

#include <memory>

#include "lddefuse.h"
#include "glibc_ops.h"
#include "FileSystemWrapper.h"

typedef std::shared_ptr<FileSystemWrapper> FileSystemWrapperPtr;
typedef std::map<std::string, FileSystemWrapperPtr> mount_point_map;


static inline mount_point_map& get_instance()
{
	static mount_point_map mount_points;
	return mount_points;
}

static inline int 
load_mount(const std::string& path, mount_point_map& mount_points, const struct FileSystemWrapper::ConfigOpts& opts)
{
	DEBUG_ENTER;
	int rv = 0;

	DEBUG_PRINT(path);

	std::map<std::string, FileSystemWrapperPtr>::iterator itr = mount_points.find(path);
	if(itr != mount_points.end()) {
		rv = EEXIST;
	} else {
		FileSystemWrapperPtr fs = FileSystemWrapperPtr(new FileSystemWrapper);
		rv = fs->init(opts);
		if(rv == 0) {
			mount_points.insert(std::make_pair(std::string(path), fs));
		}
	}

	DEBUG_EXIT(rv);
    return rv;
}

void
load_mounts(mount_point_map& mount_points)
{
	static bool load_mounts_flag = false;

	if(!load_mounts_flag) {
		DEBUG_ENTER;

		int rv = 0;
	
		load_glibc_ops();
		mount_points.clear();

		struct FileSystemWrapper::ConfigOpts fs_opts;
		bzero(&fs_opts, sizeof(struct FileSystemWrapper::ConfigOpts));
		fs_opts.backend = (char*)DEFUSE_BACKEND;
		fs_opts.mount_point = (char*)DEFUSE_MOUNT;

		rv = load_mount(std::string(DEFUSE_MOUNT), mount_points, fs_opts);

		if(rv != 0) {
			DEBUG_EXIT(rv);
			exit(rv);
		}

		// Set the mounts flag and return
		load_mounts_flag = true;

		// Restore any saved file handles
		restore_file_handles_from_shared_space();

		DEBUG_EXIT(rv);
	}
}

__attribute__((constructor))
static void init()
{
	load_mounts(get_instance());
}

FileSystemWrapper*
find_mount_and_strip_path(std::string& path)
{
	FileSystemWrapper* rv = NULL;
	mount_point_map& mount_points = get_instance();
	load_mounts(mount_points);
	if(mount_points.size() > 0) {
		for(std::map<std::string, FileSystemWrapperPtr>::iterator itr = mount_points.begin(); itr != mount_points.end(); itr++) {
			std::size_t loc = path.find(itr->first);
			if(loc != std::string::npos) {
				DEBUG_ENTER;
				DEBUG_PRINT(path);
				
				path = path.substr((itr->first).length());
				if(path.length() == 0) {
					path = "/";
				}
				rv = itr->second.get();
				DEBUG_EXIT(rv);
				break;
			}
		}
	}
	return rv;
}

FileSystemWrapper* 
find_mount_from_path(const std::string& path)
{
	DEBUG_ENTER;
	FileSystemWrapper* rv = NULL;

	DEBUG_PRINT(path);

	mount_point_map& mount_points = get_instance();
	load_mounts(mount_points);
	if(mount_points.size() > 0) {
		for(std::map<std::string, FileSystemWrapperPtr>::iterator itr = mount_points.begin(); itr != mount_points.end(); itr++) {
			std::size_t loc = path.find(itr->first);
			if(loc != std::string::npos) {
				rv = itr->second.get();
				break;
			}
		}
	}

	DEBUG_EXIT(rv);
	return rv;
}

void load_mounts(void)
{
	load_mounts(get_instance());
}
