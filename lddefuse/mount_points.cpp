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
typedef std::shared_ptr<mount_point_map> mount_point_map_ptr;

static mount_point_map_ptr mount_points;

static inline int 
load_mount(const std::string& path, mount_point_map_ptr& mount_points, const struct FileSystemWrapper::ConfigOpts& opts)
{
	DEBUG_ENTER;
	int rv = 0;

	DEBUG_PRINT(path);

	std::map<std::string, FileSystemWrapperPtr>::iterator itr = mount_points->find(path);
	if(itr != mount_points->end()) {
		rv = EEXIST;
	} else {
		FileSystemWrapperPtr fs = FileSystemWrapperPtr(new FileSystemWrapper);
		rv = fs->init(opts);
		if(rv == 0) {
			mount_points->insert(std::make_pair(std::string(path), fs));
		}
	}

	DEBUG_EXIT(rv);
    return rv;
}

static inline void
load_mounts(mount_point_map_ptr& mount_points)
{
	static bool load_mounts_flag = false;

	if(!load_mounts_flag) {
		DEBUG_ENTER;

		int rv = 0;
		mount_points = mount_point_map_ptr(new mount_point_map);

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

		DEBUG_EXIT(rv);
	}
}

__attribute__((constructor))
static void init()
{
	load_glibc_ops();
	load_mounts(mount_points);
}

FileSystemWrapper*
find_mount_and_strip_path(std::string& path)
{
	DEBUG_ENTER;
	FileSystemWrapper* rv = NULL;

	DEBUG_PRINT(path);

	if(mount_points && mount_points->size() > 0) {
		for(std::map<std::string, FileSystemWrapperPtr>::iterator itr = mount_points->begin(); itr != mount_points->end(); itr++) {
			std::size_t loc = path.find(itr->first);
			if(loc != std::string::npos) {
				path = path.substr((itr->first).length());
				if(path.length() == 0) {
					path = "/";
				}
				rv = itr->second.get();
				break;
			}
		}
	}

	DEBUG_EXIT(rv);
	return rv;
}
