#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int fd = open("/tmp/tmpdir/blah", O_RDWR|O_CREAT);
	if(fd != -1) {
		close(fd);
	}
	return 0;
}
