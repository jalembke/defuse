#ifndef __FSWRAPPER_H_
#define __FSWRAPPER_H_

#include <stdint.h>

#include <sys/types.h>
#include <sys/statvfs.h>
#include <stdio.h>

#include <set>
#include <string>

// These are file-system level operations:
class FileSystemWrapper
{
    public:

		struct ConfigOpts {
            char* backend;
			char* mount_point;
        };
    
        FileSystemWrapper() {};
        ~FileSystemWrapper() {};

		int init(const struct ConfigOpts& conf) {
			xBackend = conf.backend;
			xBackend += "/";
			return 0;
		}
        //int finalize();

		// Open the file specified by path using the open flags and mode, 
		//    returns the opened file handle in ret_fh
        int open(const char* path, int flags, mode_t mode, uint64_t* ret_fh);

		// Create a new file specified by path using the given mode
		//    returns the created file handle in ret_fh
        int create(const char* path, mode_t, uint64_t* ret_fh);

		// Close the file specified by fh
		int close(uint64_t fh);

		// Read data from the file specified by fh
        int read(uint64_t fh, char *buf, size_t size, off_t offset, size_t* bytes_read);

		// Write data to the file specified by fh
        int write(uint64_t fh, const char *buf, size_t size, off_t offset, size_t* bytes_written);

		// Synchronize the state and store of the file specified by fh
        int fsync(uint64_t fh);

		// Truncate the file specified by fh to the length specified by offset
        int ftrunc(uint64_t fh, off_t offset);

		// Retrieve attributes for the file specified by fh
        int fgetattr(uint64_t fh, struct stat *sbuf);

		// Retriev attributes for the file specified by path, flags are the same as fstatat
		//    returns the attributes in stat
        int getattr(const char* path, struct stat *stbuf, int flags);

		// Truncate the file specified by path to the length specified by offset
        int trunc(const char* path, off_t offset);

		// Change the ownership of the file specified by path
        int chown(const char* path, uid_t u, gid_t g);

		// Change the file mode bits of the file specified by path to mode
        int chmod(const char* path, mode_t mode);

		// Check the user's file permissions for the file specified by path
        int access(const char* path, int mode);

		// Rename the file specified by path to the file name specified in path_to
        int rename(const char* path, const char* path_to);
        
		// Update the access time of the file specified by path using the utimebuf
		int utime(const char* path, const struct utimbuf *ut);

		// Unlink the file specified by path
        int unlink(const char* path);

		// Create a symbolic link for the file specified by from to the file specified in path_to
		int symlink(const char *from, const char* path_to);

        int mkdir(const char* path, mode_t);
        int readdir(const char* path, std::set<std::string> *buf);
        //int readlink(const char* path, char *buf, size_t bufsize, size_t* bytes_read);
        int rmdir(const char* path);
        //int symlink(const char *from, const char* path_to);
        int statvfs(const char* path, struct statvfs *stbuf);
        int sync();
       
		/*
        int close(uint64_t fh);
        int read(uint64_t fh, char *buf, size_t size, off_t offset, size_t* bytes_read);
        int write(uint64_t fh, const char *buf, size_t size, off_t offset, size_t* bytes_written);
        int fsync(uint64_t fh);
        int ftrunc(uint64_t fh, off_t offset);
        int fgetattr(uint64_t fh, struct stat *sbuf);
		*/

	private:
		std::string xBackend;

};

#endif
