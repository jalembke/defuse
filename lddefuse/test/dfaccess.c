#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	for(int i = 0; i < 10000; i++) {
		access("/tmp/usfsal/mnt/test/test", F_OK);
	}
	return 0;
}
