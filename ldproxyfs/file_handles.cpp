#include <errno.h>
#include <map>

#include "ldproxyfs.h"
#include "file_handle_data.h"
#include "FileSystemWrapper.h"

static std::map<int, file_handle_data_ptr> file_handles;

file_handle_data::~file_handle_data()
{
	DEBUG_ENTER;

	//const std::string* path = NULL;
	//file_system->getLogicalPath(file_handle, path);
	//DEBUG_PRINT(*path);	

	int rv = file_system->close(file_handle);
	DEBUG_EXIT(rv);
}

int
insert_file_handle(int fd, file_handle_data_ptr& fhd)
{
	DEBUG_ENTER;
	int rv = 0;

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
	DEBUG_ENTER;
	int rv = 0;

	std::map<int, file_handle_data_ptr>::iterator itr = file_handles.find(fd);
	if(itr == file_handles.end()) {
		rv = ENOENT;
	} else {
		file_handles.erase(itr);
	}

	DEBUG_EXIT(rv);
    return rv;
}

struct file_handle_data*
find_file_handle(int fd)
{
	DEBUG_ENTER;
	struct file_handle_data* rv = NULL;

	std::map<int, file_handle_data_ptr>::iterator itr = file_handles.find(fd);
	if(itr != file_handles.end()) {
		rv = itr->second.get();
	}

	DEBUG_EXIT(rv);
    return rv;
}

int 
duplicate_file_handle(int fd1, int fd2)
{
	DEBUG_ENTER;
	int rv = 0;

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

	std::map<int, file_handle_data_ptr>::iterator itr = file_handles.find(fd);
	if(itr != file_handles.end()) {
		rv = itr->second.use_count();
	}

	DEBUG_EXIT(rv);
    return rv;
}
