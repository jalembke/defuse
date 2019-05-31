#include <stdio.h>
#include <time.h>
#include <utime.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
	struct utimbuf time_buffer;
	int rc = 0;
	time_t current_time = time(0);

	time_buffer.actime = current_time;
	time_buffer.modtime = current_time;

	rc = utime(argv[1], &time_buffer);
	printf("utime rc: %d\n", rc);

	return 0;
}
