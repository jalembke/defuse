#include <dirent.h>
#include <errno.h>
#include <map>
#include <memory>

#include "ldproxyfs.h"
#include "dir_handle_data.h"
#include "FileSystemWrapper.h"

static std::map<DIR*, dir_handle_data_ptr> dir_handles;

int
insert_dir_handle(DIR* dirp, dir_handle_data_ptr& dhd)
{
	DEBUG_ENTER;
	int rv = 0;

	std::map<DIR*, dir_handle_data_ptr>::iterator itr = dir_handles.find(dirp);
	if(itr != dir_handles.end()) {
		rv = EEXIST;
	} else {
		dir_handles.insert(std::make_pair(dirp, dhd));
	}

	DEBUG_EXIT(rv);
    return rv;
}

int
remove_dir_handle(DIR* dirp)
{
	DEBUG_ENTER;
	int rv = 0;

	std::map<DIR*, dir_handle_data_ptr>::iterator itr = dir_handles.find(dirp);
	if(itr == dir_handles.end()) {
		rv = ENOENT;
	} else {
		remove_file_handle(itr->second->fh_data->file_descriptor);
		dir_handles.erase(itr);
	}

	DEBUG_EXIT(rv);
    return rv;
}

dir_handle_data*
find_dir_handle(DIR* dirp)
{
	DEBUG_ENTER;
	dir_handle_data* rv = NULL;

	std::map<DIR*, dir_handle_data_ptr>::iterator itr = dir_handles.find(dirp);
	if(itr != dir_handles.end()) {
		rv = itr->second.get();
	}

	DEBUG_EXIT(rv);
    return rv;
}
