#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

static inline void create_files(int count)
{
	int fd = 0;
	char open_path[64];
	int i;

	for(i = 0; i < count; i++) {
		snprintf(open_path, sizeof(open_path), "/tmp/tmpdir/file%d", i);
		fd = open(open_path, O_RDWR|O_CREAT);
		if(fd == -1) {
			fprintf(stderr, "Unable to create file %s: %s\n", open_path, strerror(errno));
			exit(1);
		}
		close(fd);
	}
}


int main(int argc, char* argv[])
{
	struct dirent* ent_data;

	create_files(20);

	DIR* mydir = opendir("/tmp/tmpdir");
	
	ent_data = readdir(mydir);
	while(ent_data != NULL) {
		printf("%s\n", ent_data->d_name);
		ent_data = readdir(mydir);
	}

	closedir(mydir);

	return 0;
}
