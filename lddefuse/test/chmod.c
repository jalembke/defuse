#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
	for(int i = 0; i < 10000; i++) {
		chmod("/mnt/test/test", S_IRUSR);
	}
	return 0;
}
