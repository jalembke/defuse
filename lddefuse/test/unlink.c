#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[])
{
	char crtbuf[64];
	for(int i = 0; i < 10000; i++) {
		snprintf(crtbuf, sizeof(crtbuf), "/mnt/test/test%d", i);
		unlink(crtbuf);
	}
	return 0;
}
