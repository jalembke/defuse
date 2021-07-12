#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
	struct stat statbuf;
	for(int i = 0; i < 1; i++) {
		stat("/tmp/fuse/mnt/test/test", &statbuf);
	}
	return 0;
}
