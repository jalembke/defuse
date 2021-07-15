#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	char crtbuf[64];
	for(int i = 0; i < 10000; i++) {
		snprintf(crtbuf, sizeof(crtbuf), "/mnt/test/test%d", i);
		creat(crtbuf, S_IRUSR | S_IWUSR);
	}
	return 0;
}
