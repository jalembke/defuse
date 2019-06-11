#include <fcntl.h>

int main(int argc, char* argv[])
{
	int fd = open("/tmp/usfsal/dev/urandom", O_RDONLY);
	return 0;
}
