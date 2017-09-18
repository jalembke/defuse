#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string>

#include "ldproxyfs.h"

/* given a relative path, calculate complete path assuming current directory */
void resolve_path(const char *p, std::string& path)
{
	path.assign(p);

	// if first character isn't "/", prepend current working dir
	if (path[0] != '/') {
		char *cwd = get_current_dir_name();
		path = std::string(cwd) + "/" + path;
		free(cwd);
	}

	if (path.find("/./") != std::string::npos) {
		int stop = path.length()-2;
		// iterate over string... if 3 characters are /./, call replace and replace them with /
		for (int i=0; i < stop; i++) {
			if (path.substr(i,3).compare("/./") == 0) {
				path.replace(i,3,"/");
				stop = path.length()-2;
				i--;
			}
		}
	}       
			
	if (path.find("//") != std::string::npos) {
		int stop = path.length()-1;
		for (int i=0; i < stop; i++) {
			if (path.substr(i,2).compare("//") == 0) {
				path.replace(i,2,"/");
				stop = path.length()-1;
				i--;
			}
		}
	}

	if (path.find("/../") != std::string::npos) {
		// this is the difficult one....
		int stop = path.length()-3;
		int lastslash = 0;
		for (int i=0; i < stop; i++) {
			if (path.substr(i,4).compare("/../") == 0) {
				if (i == 0) {
					path.replace(0,3,"");
					stop = path.length()-3;
					i--;
				} else {
					size_t l = path.find_last_of('/', i-1);
					path.replace(l, i-l+3, "");
					stop = path.length()-3;
					i = l-1;
					// find the previous /
				}
			}
		}
	}
}

void resolve_path_at(int dirfd, const char *p, std::string& path) 
{    
    path.assign(p);
    
    // If path is absolute or relative to current working 
    //    directory, then resolve as a normal path
    if(path[0] == '/' || dirfd == AT_FDCWD) {
        resolve_path(p, path);
		return;
    }
    
	/*
    // Path is not absolute and dirfd is not AT_FDCWD so 
    //    determine the path from the list of directories
    std::map<int, DIR*>::iterator itd = fd_dirs.find(dirfd);
    if (itd != fd_dirs.end()) {
        path = (*opendirs[itd->second]->path) + path;
        //path = (*opendirs[itd->second]->path) + "/" + path;  // Removed / 
    }
    // fd is not in the list of open directories
    //    Check if it is in the list of open files and fd is a directory
    //    this will be the case if the user opened the directory with an
    //    open syscall and not an opendir
    else {
        std::map<int, plfs_file*>::iterator itf = plfs_files.find(dirfd);
        if(itf != plfs_files.end()) {
            
            // fd found in open files list, check to see if it is a directory
            //    by "stat"ing the file descriptor
            struct stat stats;
            plfs_error_t plfs_error = plfs_getattr(itf->second->fd, itf->second->path->c_str(), &stats, 0);
            
            // Error retrieving file stat information
            if(plfs_error != PLFS_SUCCESS) {
				errno = plfs_error_to_errno(plfs_error);
				return NULL;
			}
            
            // File is a directory, prepend to the relative path
            if(S_ISDIR(stats.st_mode)) {
                path = (*itf->second->path) + "/" + path;
            } 
            // File is not a directory, but must be for resolvePathAt
            else {
                errno = ENOTDIR;
                return NULL;
            }
        } 
        // File descriptor is neither a file nor a directory nor AT_FDCWD
        else {
            errno = EBADF;
            return NULL;
        }
    }
	*/
}
