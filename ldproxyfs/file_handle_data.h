#ifndef _FILE_HANDLE_DATA_H
#define _FILE_HANDLE_DATA_H

#include <string>
#include <memory>

//#include "sptr.h"

class FileSystemWrapper;

class file_handle_data {

	public:
		FileSystemWrapper* file_system;
		uint64_t file_handle;
		int file_descriptor;

		file_handle_data() :
			file_system(NULL), file_handle(0), file_descriptor(0) {}

		file_handle_data(const file_handle_data& other) :
			file_system(other.file_system), file_handle(other.file_handle), file_descriptor(other.file_descriptor) {}

		file_handle_data(FileSystemWrapper* fs, uint64_t fh, int fd) :
			file_system(fs), file_handle(fh), file_descriptor(fd) {}

		~file_handle_data();

		inline file_handle_data& operator= (const file_handle_data& other) {
			file_system = other.file_system;
			file_handle = other.file_handle;
			file_descriptor = other.file_descriptor;
			return *this;
		}

		inline bool operator== (const file_handle_data& other) {
			return (
				file_system == other.file_system && 
				file_handle == other.file_handle && 
				file_descriptor == other.file_descriptor
			);
		}
};

//typedef sptr<file_handle_data> file_handle_data_ptr;
typedef std::shared_ptr<file_handle_data> file_handle_data_ptr;

#endif // _FILE_HANDLE_DATA_H
