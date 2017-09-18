#include <stdlib.h>
#include <errno.h>
#include <strings.h>

#include <string>
#include <map>

//#include <memory>
#include "sptr.h"

#include "ldproxyfs.h"
#include "glibc_ops.h"
#include "FileSystemWrapper.h"

//typedef std::shared_ptr<FileSystemWrapper> FileSystemWrapperPtr;
//typedef std::map<std::string, FileSystemWrapperPtr> mount_point_map;
//typedef std::shared_ptr<mount_point_map> mount_point_map_ptr;

typedef sptr<FileSystemWrapper> FileSystemWrapperPtr;
typedef std::map<std::string, FileSystemWrapperPtr> mount_point_map;
typedef sptr<mount_point_map> mount_point_map_ptr;

static mount_point_map_ptr mount_points;

static inline int 
load_mount(const std::string& path)
{
	DEBUG_ENTER;
	int rv = 0;

	std::map<std::string, FileSystemWrapperPtr>::iterator itr = mount_points->find(path);
	if(itr != mount_points->end()) {
		rv = EEXIST;
	} else {
		FileSystemWrapperPtr fs = FileSystemWrapperPtr(new FileSystemWrapper);
		mount_points->insert(std::make_pair(std::string(path), fs));
	}

	DEBUG_EXIT(rv);
    return rv;
}

__attribute__ ((constructor)) void 
load_mounts(void)
{
	static bool load_mounts_flag = false;

	if(!load_mounts_flag) {
		DEBUG_ENTER;
		int rv = 0;
		
		mount_points = mount_point_map_ptr(new mount_point_map);
		rv = load_mount(std::string(PROXYFS_MOUNT));

		if(rv != 0) {
			DEBUG_EXIT(rv);
			exit(rv);
		}

		load_mounts_flag = true;

		DEBUG_EXIT(rv);
	}
}

FileSystemWrapper*
find_mount_and_strip_path(std::string& path)
{
	DEBUG_ENTER;
	FileSystemWrapper* rv = NULL;

	DEBUG_PRINT(path);

	load_glibc_ops();

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
