#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int fd = openat(AT_FDCWD, "/tmp/usfsal/tmp/test", O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(fd != -1) {
		close(fd);
		return 0;
	}
	return 1;
}
