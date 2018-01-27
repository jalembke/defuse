#ifndef _DIR_HANDLE_DATA_H
#define _DIR_HANDLE_DATA_H

#include <string>
#include <set>
#include <memory>

//#include "sptr.h"
#include "file_handle_data.h"

class dir_handle_data {

	public:
		file_handle_data_ptr fh_data;
		std::set<std::string> dir_entries;
		std::set<std::string>::iterator dir_pointer;

		dir_handle_data() :
			fh_data(), dir_entries(), dir_pointer() {}

	private:
		dir_handle_data(const dir_handle_data& other) :
			fh_data(other.fh_data), dir_entries(other.dir_entries), dir_pointer(other.dir_pointer) {}
};

//typedef sptr<dir_handle_data> dir_handle_data_ptr;
typedef std::shared_ptr<dir_handle_data> dir_handle_data_ptr;

#endif // _DIR_HANDLE_DATA_H
