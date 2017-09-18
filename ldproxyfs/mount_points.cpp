#include <stdlib.h>
#include <errno.h>
#include <strings.h>

#include <string>
#include <map>

//#include <memory>
#include "sptr.h"

#include "ldhykefs.h"
#include "glibc_ops.h"
#include "HybridFS.h"

//typedef std::shared_ptr<HybridFileSystem> HybridFileSystemPtr;
//typedef std::map<std::string, HybridFileSystemPtr> mount_point_map;
//typedef std::shared_ptr<mount_point_map> mount_point_map_ptr;

typedef sptr<HybridFileSystem> HybridFileSystemPtr;
typedef std::map<std::string, HybridFileSystemPtr> mount_point_map;
typedef sptr<mount_point_map> mount_point_map_ptr;

static mount_point_map_ptr mount_points;

static inline int 
load_mount(const std::string& path, const struct HybridFileSystem::ConfigOpts& opts)
{
	DEBUG_ENTER;
	int rv = 0;

	std::map<std::string, HybridFileSystemPtr>::iterator itr = mount_points->find(path);
	if(itr != mount_points->end()) {
		rv = EEXIST;
	} else {
		HybridFileSystemPtr fs = HybridFileSystemPtr(new HybridFileSystem);
		rv = fs->init(opts);
		if(rv == 0) {
			mount_points->insert(std::make_pair(std::string(path), fs));
		}
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

		struct HybridFileSystem::ConfigOpts hykefs_opts;
		bzero(&hykefs_opts, sizeof(struct HybridFileSystem::ConfigOpts));
		hykefs_opts.backend_string = (char*)HYKEFS_BACKEND_STRING;
		hykefs_opts.max_file_size = HYKEFS_MAX_FILE_SIZE;
		hykefs_opts.max_open_kvs = HYKEFS_MAX_HANDLES;
		hykefs_opts.noatime = 1;

		rv = load_mount(std::string(PROXYFS_MOUNT), hykefs_opts);

		if(rv != 0) {
			DEBUG_EXIT(rv);
			exit(rv);
		}

		load_mounts_flag = true;

		DEBUG_EXIT(rv);
	}
}

HybridFileSystem*
find_mount_and_strip_path(std::string& path)
{
	DEBUG_ENTER;
	HybridFileSystem* rv = NULL;

	DEBUG_PRINT(path);

	load_glibc_ops();

	if(mount_points && mount_points->size() > 0) {
		for(std::map<std::string, HybridFileSystemPtr>::iterator itr = mount_points->begin(); itr != mount_points->end(); itr++) {
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
