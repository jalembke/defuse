#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "usfsal.h"
#include "file_handle_data.h"
//#include "util.h"

static inline void errno_exit(char* message)
{
	fprintf(stderr, "%s : %s\n", message, strerror(errno));
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	struct file_handle_data mydata;
	mydata.file_handle = 0;

	if(insert_file_handle(0, &mydata) != 0) {
		errno_exit("insert_file_handle");
	}
	if(duplicate_file_handle(0, 3) != 0) {
		errno_exit("duplicate_file_handle 1");
	}
	if(duplicate_file_handle(1, 3) != EBADF) {
		errno_exit("duplicate_file_handle 2");
	}
	if(duplicate_file_handle(0, 3) != 0) {
		errno_exit("duplicate_file_handle 3");
	}
	if(remove_file_handle(0) != 0) {
		errno_exit("remove_file_handle 1");
	}
	if(remove_file_handle(0) != ENOENT) {
		errno_exit("remove_file_handle 2");
	}
	if(remove_file_handle(3) != 0) {
		errno_exit("remove_file_handle 3");
	}

	return 0;
}
